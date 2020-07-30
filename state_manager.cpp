#include "state_manager.h"

#include <Arduino.h>
#include <SD.h>

#include "sys_config.h"
#include "debug_serial.h"

namespace scottz0r
{
namespace state
{
    constexpr uint16_t no_index_value = (1 << 16) - 1;

    bool get_state(State& state)
    {
        state = {no_index_value, no_index_value, no_index_value, no_index_value, no_index_value, 0};

        if(!SD.exists(state_filename))
        {
            return false;
        }

        auto file = SD.open(state_filename, FILE_READ);
        if(!file)
        {
            return false;
        }

        auto bytes_read = file.read((uint8_t*)&state, sizeof(state));

        DEBUG_PRINT("Last 0: ");
        DEBUG_PRINTLN((int)state.last_idx_0);
        DEBUG_PRINT("Last 1: ");
        DEBUG_PRINTLN((int)state.last_idx_1);
        DEBUG_PRINT("Last 2: ");
        DEBUG_PRINTLN((int)state.last_idx_2);
        DEBUG_PRINT("Last 3: ");
        DEBUG_PRINTLN((int)state.last_idx_3);
        DEBUG_PRINT("Last 4: ");
        DEBUG_PRINTLN((int)state.last_idx_4);
        DEBUG_PRINT("Count: ");
        DEBUG_PRINTLN((int)state.count);

        file.close();
        return bytes_read == sizeof(state);
    }

    bool set_state(const State& state)
    {
        if(SD.exists(state_filename))
        {
            SD.remove(state_filename);
        }

        auto file = SD.open(state_filename, FILE_WRITE);
        if(!file)
        {
            return false;
        }

        auto bytes_written = file.write((uint8_t*)&state, sizeof(state));
        file.close();

        return bytes_written == sizeof(state);
    }

    bool has_been_shown_lately(uint16_t number, const State& state)
    {
        if(number == state.last_idx_0)
        {
            return true;
        }

        if(number == state.last_idx_1)
        {
            return true;
        }

        if(number == state.last_idx_2)
        {
            return true;
        }

        if(number == state.last_idx_3)
        {
            return true;
        }

        if(number == state.last_idx_4)
        {
            return true;
        }

        return false;
    }

    void set_shown_lately(uint16_t index, State& state)
    {
        // Shift everything "down"
        state.last_idx_4 = state.last_idx_3;
        state.last_idx_3 = state.last_idx_2;
        state.last_idx_2 = state.last_idx_1;
        state.last_idx_1 = state.last_idx_0;

        // Add new value to 0-th entry.
        state.last_idx_0 = index;
    }
}
}
