#ifndef _SCOTTZ0R_CONFIG_MANAGER_H_INCLUDE_GUARD
#define _SCOTTZ0R_CONFIG_MANAGER_H_INCLUDE_GUARD

namespace scottz0r
{
namespace config
{
    static constexpr char* config_filename = "CFG.TXT";

    struct WebConfig
    {
        char wifi_ssid[64];
        char wifi_pwd[64];
        char cfg_host[128];
        int cfg_port;
        char cfg_path[256];
    };

    bool load_config(WebConfig& config);
}}

#endif // _SCOTTZ0R_CONFIG_MANAGER_H_INCLUDE_GUARD
