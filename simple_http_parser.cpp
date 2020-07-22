#include "simple_http_parser.hpp"

namespace scottz0r
{
namespace http
{
    StringSlice get_body(const StringSlice& response) noexcept
    {
        auto buffer = response;

        // Loop until an empty line is found. The empty line marks the start of the body.
        StringSlice line = get_line(buffer);
        while (line.rstrip().size() > 0)
        {
            buffer = buffer.substr(line.size());
            line = get_line(buffer);
        }

        buffer = buffer.substr(line.size());
        return buffer;
    }

    /// @brief Get the next http header and update the read position in the buffer.
    ///
    /// @param buffer String slice to read header from.
    /// @return Slice containing the header. Will be empty if no more headers present. Header will not contain newline
    ///         characters.
    StringSlice get_next_header(StringSlice& buffer) noexcept
    {
        auto line = get_line(buffer);

        // Advance the buffer past the header just read.
        buffer = buffer.substr(line.size());

        // Only new line, which means the body is next and no more headers.
        if (line.rstrip().empty())
        {
            return StringSlice();
        }

        // Return the header line.
        return line.rstrip();
    }

    int get_status_code(const StringSlice& response) noexcept
    {
        if (response.substr(0, 4) != "HTTP")
        {
            return -1;
        }

        auto first_space = response.find(' ');
        if (first_space == StringSlice::npos)
        {
            return -1;
        }

        ++first_space;

        auto second_space = response.find(' ', first_space);

        StringSlice status_slice;

        if (second_space != StringSlice::npos)
        {
            status_slice = response.substr(first_space, second_space - first_space);
        }
        else
        {
            status_slice = response.substr(first_space);
        }

        return to_int(status_slice);
    }
}
}
