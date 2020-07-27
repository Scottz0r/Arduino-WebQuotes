#include "config_manager.h"

#include <Arduino.h>
#include <SD.h>

#include "string_slice.h"
#include "debug_serial.h"

namespace scottz0r
{
namespace config
{
    static void copy_slice_to(char* dst, std::size_t dst_size, const StringSlice& slice);

    bool load_config(WebConfig& config)
    {
        DEBUG_PRINTLN("Attempting to load configuration...");

        if(!SD.exists(config_filename))
        {
            DEBUG_PRINTLN("Config file not found!");
            return false;
        }

        auto file = SD.open(config_filename, FILE_READ);
        if(!file)
        {
            DEBUG_PRINTLN("Config file was not opened!");
            return false;
        }
        
        char buffer[256];
        std::size_t bytes_read;
        StringSlice slice;

        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        slice = StringSlice(buffer, bytes_read).rstrip();
        copy_slice_to(config.wifi_ssid, sizeof(config.wifi_ssid), slice);

        // Password
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        slice = StringSlice(buffer, bytes_read).rstrip();
        copy_slice_to(config.wifi_pwd, sizeof(config.wifi_pwd), slice);

        // Host
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        slice = StringSlice(buffer, bytes_read).rstrip();
        copy_slice_to(config.cfg_host, sizeof(config.cfg_host), slice);

        // Port
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        buffer[bytes_read] = 0;
        config.cfg_port = atol(buffer);

        // Path
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        slice = StringSlice(buffer, bytes_read).rstrip();
        copy_slice_to(config.cfg_path, sizeof(config.cfg_path), slice);

        DEBUG_PRINTLN("Config loaded successfully!");
        return true;
    }

    static void copy_slice_to(char* dst, std::size_t dst_size, const StringSlice& slice)
    {
        std::size_t i;
        for(i = 0; i < dst_size && i < slice.size(); ++i)
        {
            dst[i] = slice[i];
        }

        if(i < dst_size)
        {
            dst[i] = 0;
        }
        else
        {
            dst[dst_size - 1] = 0;
        }
    }
}}
