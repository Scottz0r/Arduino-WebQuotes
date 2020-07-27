#include "quote_manager.h"

#include <Arduino.h>
#include <SD.h>

#include "sys_config.h"
#include "debug_serial.h"

namespace scottz0r
{
    QuoteManager::QuoteManager()
        : m_ready( false )
    {
    }

    bool QuoteManager::load_quote(std::size_t index)
    {
        DEBUG_PRINTLN("Trying to load a quote");

        if(!SD.exists(data_filename))
        {
            return false;
        }

        auto file = SD.open(data_filename, FILE_READ);
        if(!file)
        {
            return false;
        }

        std::size_t i = 0;

        while(file.available() && i < index)
        {
            DEBUG_PRINTLN("Skipping quote");
            auto bytes_read = file.readBytesUntil('\n', m_buffer, sizeof(m_buffer));
            ++i;
        }

        DEBUG_PRINTLN("Found quote");

        auto bytes_read = file.readBytesUntil('\n', m_buffer, sizeof(m_buffer));
        if(bytes_read == 0)
        {
            return false;
        }

        m_data_size = bytes_read;

        if(bytes_read < sizeof(m_buffer))
        {
            m_buffer[bytes_read] = 0;
        }
        else
        {
            m_buffer[sizeof(m_buffer) - 1] = 0;
        }

        DEBUG_PRINTLN("Quote found!");
        DEBUG_PRINT("Quote is: ");
        DEBUG_PRINTLN(m_buffer);

        m_ready = true;
        file.close();
        return true;
    }

    int QuoteManager::get_quote_count()
    {
        DEBUG_PRINTLN("Counting quotes...");

        if(!SD.exists(data_filename))
        {
            return 0;
        }

        auto file = SD.open(data_filename, FILE_READ);
        if(!file)
        {
            return 0;
        }

        std::size_t i = 0;
        while(file.available())
        {
            DEBUG_PRINTLN("Skipping quote");
            auto bytes_read = file.readBytesUntil('\n', m_buffer, sizeof(m_buffer));

            if(bytes_read > 0)
            {
                StringSlice slice(m_buffer, bytes_read);
                if(!slice.strip().empty())
                {
                    ++i;
                }
            }
        }

        return i;
    }

    StringSlice QuoteManager::get_name() const
    {
        if(!m_ready)
        {
            return StringSlice();
        }

        StringSlice buffer(m_buffer, m_data_size);

        auto pos = buffer.find('|');
        if (pos != StringSlice::npos)
        {
            return buffer.substr(0, pos).strip();
        }

        return StringSlice();
    }

    StringSlice QuoteManager::get_quote() const
    {
        if(!m_ready)
        {
            return StringSlice();
        }

        StringSlice buffer(m_buffer, m_data_size);

        auto pos = buffer.find('|');
        if (pos != StringSlice::npos)
        {
            return buffer.substr(pos + 1).strip();
        }

        return StringSlice();
    }
}
