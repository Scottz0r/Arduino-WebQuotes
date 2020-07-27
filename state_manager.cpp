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

        char buffer[32];
        auto bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read < sizeof(buffer))
        {
            buffer[bytes_read] = 0;
        }
        else
        {
            buffer[sizeof(buffer) - 1] = 0;
        }
        
        state.last_idx = atoi(buffer);

        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read < sizeof(buffer))
        {
            buffer[bytes_read] = 0;
        }
        else
        {
            buffer[sizeof(buffer) - 1] = 0;
        }

        state.count = atoi(buffer);

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

        file.print(state.last_idx);
        file.print("\r\n");
        file.print(state.count);
        file.print("\r\n");

        file.close();
    }
}
}
