#include "quote_manager.h"

#include <Arduino.h>
#include <SD.h>

#include "stream_util.h"
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

        // Skip first line, which has count:
        skip_line(file);

        std::size_t i = 0;

        while(file.available() && i < index)
        {
            skip_line(file);
            ++i;
        }

        DEBUG_PRINTLN("Found quote");

        bool truncated;
        auto bytes_read = get_line(file, m_buffer, sizeof(m_buffer), truncated);
        if(bytes_read == 0)
        {
            return false;
        }

        m_data_size = bytes_read;

        DEBUG_PRINTLN("Quote found!");
        DEBUG_PRINT("Quote is: ");
        DEBUG_PRINTLN(m_buffer);

        m_ready = true;
        file.close();
        return true;
    }

    unsigned QuoteManager::get_quote_count()
    {
        DEBUG_PRINTLN("Getting quote count...");

        if(!SD.exists(data_filename))
        {
            return 0;
        }

        auto file = SD.open(data_filename, FILE_READ);
        if(!file)
        {
            return 0;
        }

        // First line has count of quotes.
        unsigned count = 0;
        bool truncated = false;
        auto bytes_read =  get_line(file, m_buffer, sizeof(m_buffer), truncated);
        if(bytes_read > 0 && !truncated)
        {
            count = (unsigned)atoi(m_buffer);
        }

        return count;
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
