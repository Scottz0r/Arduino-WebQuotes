#include <Arduino.h>
#include <string.h>
#include <SD.h>
#include <StringSlice.h>

#include "config_manager.h"
#include "debug_serial.h"
#include "eink_display.h"
#include "logging.h"
#include "prj_pins.h"
#include "state_manager.h"
#include "quote_manager.h"
#include "web_downloader.h"

using namespace scottz0r;

QuoteManager quote_manager;

void setup()
{
#ifndef NDEBUG
    Serial.begin(115200);
#endif

    DEBUG_PRINTLN("Starting!");

    display::begin();

    DEBUG_PRINTLN("Initializing SD...");
    if(!SD.begin(SD_CS))
    {
        display::display_error("SD card missing.");
        deep_sleep();
        return;
    }

    // Start logger, which must be done after SD initialization.
    Log.begin();

    // Load config, must be after SD initialization. Load fail can be recovered from.
    if(!config.load())
    {
        DEBUG_PRINTLN("Config load failed, but program execution will continue.");
        Log.println("Config load failed.");
    }

    program_main();
    
    deep_sleep();
}

// Deep sleep, which acts like an "exit" function. All paths will eventually end up here. This will put the processor
// in a low power deep sleep state, and will wakeup with the reset pin.
void deep_sleep()
{
    // TODO: Configure Deep sleep time?
    // Go to deep sleep. Requires pin 16 to be wired to reset. 60e6 = 1 minute.
    DEBUG_PRINTLN("Going to sleep...");

    // Flush log.
    Log.close();

    constexpr auto sleep_time = (uint64_t)60e6 * 60;
    ESP.deepSleep(sleep_time);
}

void program_main()
{
    // Get max refresh count from config. Assumes this is defaulted to a reasonable number upon config failure.
    int max_count = config.get().refresh_count;

    state::State pgm_state;
    state::get_state(pgm_state);

    auto num_quotes = quote_manager.get_quote_count();
    DEBUG_PRINT("Number of quotes on SD card: ");
    DEBUG_PRINTLN(num_quotes);
    
    DEBUG_PRINT("Current count: ");
    DEBUG_PRINTLN(pgm_state.count);

    // No quotes = no file, or maximum number of refreshes expired. Go fetch a new file.
    if(num_quotes == 0 || pgm_state.count >= max_count)
    {
        DEBUG_PRINTLN("No quotes found or need new quotes...");
        if(!web::download_file() && num_quotes == 0)
        {
            // If a file could not be downloaded, and there are 0 quotes from an existing file, then hard error
            // because there is nothing to display.
            DEBUG_PRINTLN("Quote download failed!");
            display::display_error("Download failed. Communication error or file corrupt.");
            Log.println("Download failed.");
            return;
        }
        else
        {
            Log.println("Download successful.");

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

    // Get random number from ESP library (should use analog pin to get noise?)
    constexpr auto max_loops = 100;
    uint32_t rng;
    std::size_t random_idx;
    bool shown_lately;
    int ctr = 0;

    do
    {
        rng = ESP.random();
        random_idx = rng % num_quotes;
        shown_lately = state::has_been_shown_lately((uint16_t)random_idx, pgm_state);
        ++ctr;

        DEBUG_PRINT("Random number ");
        DEBUG_PRINT((int)random_idx);
        DEBUG_PRINT(", Shown = ");
        DEBUG_PRINTLN((int)shown_lately);
    }
    while(shown_lately && ctr < max_loops);

    // Try to get the quote:
    if(!quote_manager.load_quote(random_idx))
    {
        DEBUG_PRINTLN("Quote load failed!");
        // No quote, but a file exists. Cannot recover at this point. Update state to force a re-download next time,
        // then give up on life.
        pgm_state.count = max_count + 42;
        state::set_state(pgm_state);
        display::display_error("Something went very wrong.");
        Log.println("Quote load failed.");
        return;
    }

    DEBUG_PRINTLN("Got quote. Now displaying...");
    StringSlice text = quote_manager.get_quote();
    StringSlice name = quote_manager.get_name();
    display::display_quote(text, name);

    // Save state.
    DEBUG_PRINTLN("Saving state...");
    state::set_shown_lately((uint16_t)random_idx, pgm_state);
    pgm_state.count += 1;
    state::set_state(pgm_state);

    Log.print("Quote cycle successful with quote #");
    Log.print((int)random_idx);
    Log.println(".");;
}

void loop()
{
    // Do nothing.
}
