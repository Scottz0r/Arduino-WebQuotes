#ifndef _SCOTTZ0R_SIMPLE_HTTP_PARSER_INCLUDE_GUARD
#define _SCOTTZ0R_SIMPLE_HTTP_PARSER_INCLUDE_GUARD

#include "string_slice.h"

namespace scottz0r
{
namespace http
{

    StringSlice get_body(const StringSlice& response) noexcept;

    StringSlice get_next_header(StringSlice& buffer) noexcept;

    int get_status_code(const StringSlice& response) noexcept;
}
}

#endif // _SCOTTZ0R_SIMPLE_HTTP_PARSER_INCLUDE_GUARD
