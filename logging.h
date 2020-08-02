#ifndef _SCOTTZ0R_LOGGING_H_INCLUDE_GUARD
#define _SCOTTZ0R_LOGGING_H_INCLUDE_GUARD

#include <SD.h>

namespace scottz0r
{
    class Logger
    {
    public:
        Logger();

        void begin();

        void close();
    
        void print(const char* msg);

        void print(int num);

        void println(const char* msg);

    private:
        fs::File m_file;
    };

    // Single instance log class.
    extern Logger Log;
}

#endif // _SCOTTZ0R_LOGGING_H_INCLUDE_GUARD
