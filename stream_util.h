#ifndef _SCOTTZ0R_STREAM_UTIL_H_INCLUDE_GUARD
#define _SCOTTZ0R_STREAM_UTIL_H_INCLUDE_GUARD

#include <Arduino.h>
#include <stddef.h>

namespace scottz0r
{
    std::size_t get_line(Stream& stream, char* buffer, std::size_t size, bool& truncated);

    void skip_line(Stream& stream);
}

#endif // _SCOTTZ0R_STREAM_UTIL_H_INCLUDE_GUARD
