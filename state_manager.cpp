#include "state_manager.h"

#include <Arduino.h>
#include <SD.h>

#include "sys_config.h"
#include "debug_serial.h"

namespace scottz0r
{
namespace state
{
    bool get_state(State& state)
    {
        state = {};

        if(!SD.exists(state_filename))
        {
            return false;
        }

        auto file = SD.open(state_filename, FILE_READ);
        if(!file)
        {
            return false;
        }

        // TODO: This is unsafe.
        char buffer[32];
        auto bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        buffer[bytes_read] = 0;

        state.next_idx = atol(buffer);

        file.close();
        return true;
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

        file.print(state.next_idx);
        file.print("\r\n");

        file.close();
    }
}
}
