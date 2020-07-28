#ifndef _SCOTTZ0R_EINK_DISPLAY_H_INCLUDE_GUARD
#define _SCOTTZ0R_EINK_DISPLAY_H_INCLUDE_GUARD

#include "string_slice.h"

namespace scottz0r
{
namespace display
{
    void begin();

    void display_quote(const StringSlice& text, const StringSlice& name);

    void display_error(const char* errmsg);
}
}

#endif // _SCOTTZ0R_EINK_DISPLAY_H_INCLUDE_GUARD
