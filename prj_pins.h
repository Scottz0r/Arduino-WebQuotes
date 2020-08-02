#ifndef _SCOTTZ0R_PRJ_PINS_H_INCLUDE_GUARD
#define _SCOTTZ0R_PRJ_PINS_H_INCLUDE_GUARD

namespace scottz0r
{
    // E-Ink configuration
    constexpr auto SD_CS = 2;
    constexpr auto SRAM_CS = -1; // Cannot use because on same pin as reset.
    constexpr auto EPD_CS = 0;
    constexpr auto EPD_DC = 15;
    constexpr auto EPD_RESET = -1;
    constexpr auto EPD_BUSY = -1; // No connected on featherwing.
}

#endif // _SCOTTZ0R_PRJ_PINS_H_INCLUDE_GUARD
