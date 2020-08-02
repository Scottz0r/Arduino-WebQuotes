#ifndef _SCOTTZ0R_CONFIG_MANAGER_H_INCLUDE_GUARD
#define _SCOTTZ0R_CONFIG_MANAGER_H_INCLUDE_GUARD

namespace scottz0r
{
    struct WebConfig
    {
        int refresh_count;
        char wifi_ssid[64];
        char wifi_pwd[64];
        char cfg_host[128];
        int cfg_port;
        char cfg_path[128];
    };

    class ConfigManager
    {
    public:
        ConfigManager();

        bool load();

        const WebConfig& get() const;

        explicit operator bool() const;

    private:
        void _set_defaults();

        bool _try_load();

        WebConfig m_values;
        bool m_is_loaded;
    };

    extern ConfigManager config;
}

#endif // _SCOTTZ0R_CONFIG_MANAGER_H_INCLUDE_GUARD
