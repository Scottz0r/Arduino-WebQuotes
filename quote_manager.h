#ifndef _SCOTTZ0R_QUOTE_MANAGER_H_INCLUDE_GUARD
#define _SCOTTZ0R_QUOTE_MANAGER_H_INCLUDE_GUARD

#include <stddef.h>
#include <StringSlice.h>
#include <SD.h>

namespace scottz0r
{
    class QuoteManager
    {
    public:
        QuoteManager();

        bool load_quote(std::size_t index);

        unsigned get_quote_count();

        StringSlice get_name() const;

        StringSlice get_quote() const;

    private:
        bool _try_get_quote(fs::File& file, std::size_t index);

        bool m_ready;

        char m_name[32];
        unsigned m_name_size;
        
        char m_quote[124];
        unsigned m_quote_size;
    };

    bool validate_file(const char* filename);
}

#endif // _SCOTTZ0R_QUOTE_MANAGER_H_INCLUDE_GUARD
