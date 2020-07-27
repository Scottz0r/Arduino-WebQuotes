#include <Arduino.h>
#include <string.h>
#include <SD.h>

#include <Adafruit_GFX.h>
#include <Adafruit_EPD.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "string_slice.h"
#include "quote_manager.h"
#include "web_downloader.h"
#include "state_manager.h"

#include "debug_serial.h"

using namespace scottz0r;

// E-Ink configuration
constexpr auto SD_CS = 2;
constexpr auto SRAM_CS = -1; // Cannot use because on same pin as reset.
constexpr auto EPD_CS = 0;
constexpr auto EPD_DC = 15;
constexpr auto EPD_RESET = -1;
constexpr auto EPD_BUSY = -1; // No connected on featherwing.

Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

QuoteManager quote_manager;

void setup()
{
#ifndef NDEBUG
    Serial.begin(115200);
#endif
    DEBUG_PRINTLN("Starting!");

    DEBUG_PRINTLN("Initializing Display...");
    display.begin();

    DEBUG_PRINTLN("Initializing SD...");
    SD.begin(SD_CS);

    program_main();

    // TODO: Configure Deep sleep time?
    // Go to deep sleep. Requires pin 16 to be wired to reset. 60e6 = 1 minute.
    DEBUG_PRINTLN("Going to sleep...");

    constexpr auto sleep_time = (uint64_t)60e6 * 60;
    ESP.deepSleep(sleep_time);
}

void program_main()
{
    // TODO: Make this config value:
    // constexpr int max_count = 24 * 7; // 24 times a day, 7 days (download every week)
    constexpr int max_count = 24 * 1; // 24 times a day, 1 day (download every day)

    state::State pgm_state;
    state::get_state(pgm_state);

    auto num_quotes = quote_manager.get_quote_count();
    DEBUG_PRINT("Number of quotes on SD card: ");
    DEBUG_PRINTLN(num_quotes);
    
    DEBUG_PRINT("Current count: ");
    DEBUG_PRINTLN(pgm_state.count);

    DEBUG_PRINT("Last Index: ");
    DEBUG_PRINTLN(pgm_state.last_idx);

    // No quotes = no file, or maximum number of refreshes expired. Go fetch a new file.
    if(num_quotes == 0 || pgm_state.count >= max_count)
    {
        DEBUG_PRINTLN("No quotes found or need new quotes...");
        if(!web::download_file())
        {
            DEBUG_PRINTLN("Quote download failed!");
            // TODO: Error - display failure? Can recover if file still there?
        }
        else
        {
            // Reset count state on a successful download only.
            pgm_state.count = 0;

            // Update number of quotes from newly downloaded file.
            num_quotes = quote_manager.get_quote_count();
        }
    }
    else
    {
        DEBUG_PRINTLN("Web download not required!");
    }

    DEBUG_PRINTLN("Generating random index...");

    // WiFi must be enabled for this:
    std::size_t random_idx;
    auto rng = ESP.random();
    random_idx  = rng % (unsigned)num_quotes;
    while(random_idx == pgm_state.last_idx)
    {
        random_idx  = rng % (unsigned)num_quotes;
    }

    DEBUG_PRINT("Random index: ");
    DEBUG_PRINTLN(random_idx);

    if(quote_manager.load_quote(random_idx))
    {
        DEBUG_PRINTLN("Got quote. Now displaying...");
        StringSlice text = quote_manager.get_quote();
        StringSlice name = quote_manager.get_name();
        wrap_and_display(text, name);

        // Save state.
        DEBUG_PRINTLN("Saving state...");
        pgm_state.last_idx = random_idx;
        pgm_state.count += 1;
        state::set_state(pgm_state);
    }
    else
    {
        DEBUG_PRINTLN("Quote load failed!");
        // TODO: Error - no quote. Cannot really recover from this?
    }
}

static StringSlice get_word(StringSlice& buffer)
{
    // TODO: Find ,.;:\n etc.
    auto pos = buffer.find(' ');

    if (pos != StringSlice::npos)
    {
        // If word found, return the word and jump the input buffer after the word and whitespace/punctuation.
        auto word = buffer.substr(0, pos);
        buffer = buffer.substr(pos + 1);
        return word;
    }
    else
    {
        // When on the last word, set the buffer to empty and return the contents of the buffer.
        auto word = buffer;
        buffer = StringSlice();
        return word;
    }
}

static void wrap_and_display(const StringSlice& text, const StringSlice& name)
{
    DEBUG_PRINTLN("Displaying start!");

    StringSlice ss = text;

    display.clearBuffer();

    DEBUG_PRINTLN("Chunking and wrapping words...");

    constexpr auto word_buffer_size = 64;
    char word_buffer[word_buffer_size];

    constexpr auto top_offset = 16; // pixels

    display.setTextColor(EPD_BLACK);
    display.setFont(&FreeSerif9pt7b);
    display.setTextSize(1);
    display.setTextWrap(false);
    display.setCursor(0, top_offset);

    int16_t x1, y1;
    uint16_t width, height;

    while(!ss.empty())
    {
        // TODO: Better buffer copying?
        auto word = get_word(ss);
        if(word.size() < word_buffer_size)
        {
            memcpy(word_buffer, word.data(), word.size());
            word_buffer[word.size()] = 0;
        }
    
        display.getTextBounds(word_buffer, display.getCursorX(), display.getCursorY(), &x1, &y1, &width, &height);

        if(x1 + width > display.width())
        {
            // Wrap using GFX's library newline handling.
            display.write('\r');
            display.write('\n');
        }

        display.print(word_buffer);

        // Write a space after each word.
        display.write(' ');
    }

    DEBUG_PRINTLN("Formatting name...");

    // Write the name in the bottom right. Draw a line above this.
    if(name.size() < word_buffer_size)
    {
        memcpy(word_buffer, name.data(), name.size());
        word_buffer[name.size()] = 0;
    }
    else
    {
        memcpy(word_buffer, name.data(), word_buffer_size);
        word_buffer[word_buffer_size - 1] = 0;
    }

    display.setTextColor(EPD_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextSize(1);
    display.getTextBounds(word_buffer, 0, 0, &x1, &y1, &width, &height);
    
    auto name_x = display.width() - width - 6; // Number: add padding from the right
    auto name_y = display.height() - 6; // 6px bottom padding (cursor position is bottom of text).
    display.setCursor(name_x, name_y);
    display.print(word_buffer);

    // Line:
    auto line_y = name_y - height - 4;
    display.drawFastHLine(0, line_y, display.width(), EPD_BLACK);

    display.display();

    DEBUG_PRINTLN("Displaying done!");
}

void loop()
{
    // Do nothing.
}
