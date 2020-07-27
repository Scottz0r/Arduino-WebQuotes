#ifndef _SCOTTZ0R_QUOTE_MANAGER_H_INCLUDE_GUARD
#define _SCOTTZ0R_QUOTE_MANAGER_H_INCLUDE_GUARD

#include <stddef.h>
#include "string_slice.h"

namespace scottz0r
{
    class QuoteManager
    {
    public:
        QuoteManager();

        bool load_quote(std::size_t index);

        // TODO - get number of quotes.

        StringSlice get_name() const;

        StringSlice get_quote() const;

    private:
        bool m_ready;
        char m_buffer[256];
        std::size_t m_data_size;
    };
}

#endif // _SCOTTZ0R_QUOTE_MANAGER_H_INCLUDE_GUARD
