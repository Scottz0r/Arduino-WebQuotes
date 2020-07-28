#include "eink_display.h"

#include <Adafruit_GFX.h>
#include <Adafruit_EPD.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "debug_serial.h"
#include "prj_pins.h"

namespace scottz0r
{
namespace display
{
    Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

    static StringSlice get_word(StringSlice& buffer);
    
    void begin()
    {
        DEBUG_PRINTLN("Initializing Display...");
        display.begin();
    }

    void display_quote(const StringSlice& text, const StringSlice& name)
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

    void display_error(const char* errmsg)
    {
        display.clearBuffer();

        DEBUG_PRINTLN("Writing error...");

        display.setTextColor(EPD_BLACK);
        display.setFont(nullptr);
        display.setTextSize(1);
        display.setTextWrap(true);
        display.setCursor(0, 0);

        display.print("ERROR: ");
        display.print(errmsg);

        display.display();
    }

    static StringSlice get_word(StringSlice& buffer)
    {
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
}
}
