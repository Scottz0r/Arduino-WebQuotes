#ifndef _SCOTTZ0R_SYS_CONFIG_H_INCLUDE_GUARD
#define _SCOTTZ0R_SYS_CONFIG_H_INCLUDE_GUARD

namespace scottz0r
{
    // "Temporary" filename to download
    constexpr auto download_filename = "DOWNLD.BIN";

    // Data file for quotes.
    constexpr auto data_filename = "DATA.BIN";

    // State file name
    constexpr auto state_filename = "STATE.BIN";

    // Configuration file name.
    constexpr char* config_filename = "CFG.TXT";

    // Default refresh interval count used when config load fails.
    constexpr int refresh_count = 24 * 7; // 1 week, assuming 1 hour intervals.

    constexpr auto log_filename = "LOG.TXT";
}

#endif // _SCOTTZ0R_SYS_CONFIG_H_INCLUDE_GUARD
