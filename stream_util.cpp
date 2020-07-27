#include "stream_util.h"

namespace scottz0r
{
    /// @brief Read a line from the stream into the buffer. If the buffer is not large enough to hold the entire line,
    ///        the stream will still be read up to a newline character and the line will be truncated.
    ///        Output will always be null terminated.
    std::size_t get_line(Stream& stream, char* buffer, std::size_t size, bool& truncated)
    {
        truncated = false;

        if(size == 0)
        {
            return 0;
        }

        std::size_t count = 0;
        bool found = false;

        while(stream.available() && count < size)
        {
            char c = stream.read();
            buffer[count] = c;
            ++count;

            if(c == '\n')
            {
                found = true;
                break;
            }
        }

        // No data available. Always null terminate.
        if(count == 0)
        {
            buffer[0] = 0;
            return 0;
        }
        
        // Always null terminate.
        buffer[count] = 0;

        // If a newline was not found, read until a newline is found or no more bytes available.
        if(!found)
        {
            // Newline not found = output truncated.
            truncated = true;

            while(stream.available())
            {
                char c = stream.read();
                if(c == '\n')
                {
                    break;
                }
            }
        }

        return count;
    }

    /// @brief Read a stream until a newline character is found.
    void skip_line(Stream& stream)
    {
        while(stream.available())
        {
            char c = stream.read();
            if(c == '\n')
            {
                break;
            }
        }
    }
}
