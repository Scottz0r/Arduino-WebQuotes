#include "quote_manager.h"

#include <Arduino.h>

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

        bool success = _try_get_quote(file, index);
        m_ready = success;       

        file.close();
        return success;
    }

    bool QuoteManager::_try_get_quote(fs::File& file, std::size_t index)
    {
        constexpr auto header_bytes = 2;
        constexpr auto index_bytes = 2;

        // Read first two bytes to get total quote count. Make sure index is less than this. This assumes CPU is
        // little endian.
        if(file.available() < 2)
        {
            return false;
        }
        uint16_t total_quotes =  file.read() | (file.read() << 8);

        DEBUG_PRINT("Total quotes: ");
        DEBUG_PRINTLN((int)total_quotes);

        if(index >= total_quotes)
        {
            return false;
        }

        // Read the index for the given item. The position to seek to is 2 bytes (header) + index * 2 bytes.
        std::size_t index_pos = header_bytes + (index * index_bytes);

        DEBUG_PRINT("Index position: ");
        DEBUG_PRINTLN((int)index_pos);

        if(!file.seek(index_pos))
        {
            return false;
        }

        uint16_t quote_address = file.read() | (file.read() << 8);

        DEBUG_PRINT("Quote Address: ");
        DEBUG_PRINTLN((int)quote_address);

        if(!file.seek(quote_address))
        {
            return false;
        }

        // The first two bytes contains the count of bytes for the name and text.
        uint8_t name_size = file.read();
        uint8_t quote_size = file.read();

        DEBUG_PRINT("Name Size: ");
        DEBUG_PRINTLN(name_size);
        DEBUG_PRINT("Quote Size: ");
        DEBUG_PRINTLN(quote_size);

        // Make sure data will not cause a buffer overflow.
        if(name_size > (sizeof(m_name) - 1) || quote_size > (sizeof(m_quote) - 1))
        {
            return false;
        }

        auto actual_name_size = file.readBytes(m_name, name_size);
        auto actual_quote_size = file.readBytes(m_quote, quote_size);

        // Make sure the read sizes are actually what the data file reports.
        if(name_size != actual_name_size || quote_size != actual_quote_size)
        {
            return false;
        }

        // Null terminate inputs. Not hard required, but safer to do with character arrays.
        m_name[name_size] = 0;
        m_quote[quote_size] = 0;

        m_name_size = name_size;
        m_quote_size = quote_size;

        DEBUG_PRINT("Name: ");
        DEBUG_PRINTLN(m_name);
        DEBUG_PRINT("Quote: ");
        DEBUG_PRINTLN(m_quote);

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

        // Read first two bytes for the count.
        unsigned count = 0;
        if(file.available() >= 2)
        {
            count =  file.read() | (file.read() << 8);
        }

        file.close();
        return count;
    }

    StringSlice QuoteManager::get_name() const
    {
        if(!m_ready)
        {
            return StringSlice();
        }

        return StringSlice(m_name, m_name_size);
    }

    StringSlice QuoteManager::get_quote() const
    {
        if(!m_ready)
        {
            return StringSlice();
        }

        return StringSlice(m_quote, m_quote_size);
    }
}
