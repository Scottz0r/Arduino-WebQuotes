#include "config_manager.h"

#include <Arduino.h>
#include <SD.h>

#include <StringSlice.h>
#include "debug_serial.h"
#include "sys_config.h"

namespace scottz0r
{
    // Extern declared in config_manager.h
    ConfigManager config;

    ConfigManager::ConfigManager()
    {
        _set_defaults();
    }

    const WebConfig& ConfigManager::get() const
    {
        return m_values;
    }

    bool ConfigManager::load()
    {
        bool result = _try_load();

        if(!result)
        {
            DEBUG_PRINTLN("Configuration failed. Resetting to defaults.");
            _set_defaults();
        }
        else
        {
            m_is_loaded = true;
        }
        
        return result;
    }

    ConfigManager::operator bool() const
    {
        return m_is_loaded;
    }

    void ConfigManager::_set_defaults()
    {
        m_values.refresh_count = refresh_count; // System default value.
        m_values.wifi_ssid[0] = 0;
        m_values.wifi_pwd[0] = 0;
        m_values.cfg_host[0] = 0;
        m_values.cfg_port = 0;
        m_values.cfg_path[0] = 0;

        m_is_loaded = false;
    }

    bool ConfigManager::_try_load()
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

        // TODO: Maybe a "get line" function for a stream reference?
        // Refresh count (how many times until redownloading)
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read == 0)
        {
            return false;
        }
        buffer[bytes_read] = 0;
        m_values.refresh_count = atol(buffer);

        // WiFi SSID
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read == 0)
        {
            return false;
        }
        slice = StringSlice(buffer, bytes_read).rstrip();
        slice.copy_to(m_values.wifi_ssid);

        // WiFi Password
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read == 0)
        {
            return false;
        }
        slice = StringSlice(buffer, bytes_read).rstrip();
        slice.copy_to(m_values.wifi_pwd);

        // Host
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read == 0)
        {
            return false;
        }
        slice = StringSlice(buffer, bytes_read).rstrip();
        slice.copy_to(m_values.cfg_host);

        // Port
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read == 0)
        {
            return false;
        }
        buffer[bytes_read] = 0;
        m_values.cfg_port = atol(buffer);

        // Path
        bytes_read = file.readBytesUntil('\n', buffer, sizeof(buffer));
        if(bytes_read == 0)
        {
            return false;
        }
        slice = StringSlice(buffer, bytes_read).rstrip();
        slice.copy_to(m_values.cfg_path);

        DEBUG_PRINTLN("Config loaded successfully!");
        return true;
    }
}
