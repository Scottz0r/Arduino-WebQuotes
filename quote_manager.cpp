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

        unsigned i = 0;
        while(file.available())
        {
            // Assumes file format is valid.
            skip_line(file);
            ++i;
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

    bool validate_file(const char* filename)
    {
        // 63 chars for name, 1 char for pipe, 127 chars for text, carriage return and newline.
        constexpr auto max_line_length = 193;
        constexpr auto max_name_size = 63;

        DEBUG_PRINTLN("Validating file...");

        auto file = SD.open(filename);
        if(!file)
        {
            return false;
        }

        char buffer[256];
        bool success = true;

        while(file.available() && success)
        {
            // Line must be less than maximum.
            bool truncated = false;
            auto line_size = get_line(file, buffer, sizeof(buffer), truncated);
            if(line_size > max_line_length || truncated)
            {
                DEBUG_PRINTLN("Line too big.");
                success =  false;
            }

            // Only the last line can be empty.
            StringSlice line_slice(buffer, line_size);
            if(line_slice.strip().empty())
            {
                DEBUG_PRINTLN("Empty line.");
                success = false;
            }

            // Test that name is within bounds.
            auto pos = line_slice.find('|');
            if(pos == StringSlice::npos)
            {
                DEBUG_PRINTLN("Pipe not found on line.");
                success = false;
            }
            else
            {
                auto name_slice = line_slice.substr(0, pos);
                if(name_slice.size() > max_name_size)
                {
                    DEBUG_PRINTLN("Name too large.");
                    success = false;
                }
            }
        }

        file.close();
        return success;
    }
}
