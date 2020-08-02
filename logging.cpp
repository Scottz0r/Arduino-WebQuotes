#include "logging.h"
#include "sys_config.h"

namespace scottz0r
{
    // Extern declared in logging.h
    Logger Log;

    Logger::Logger()
    {
    }

    void Logger::begin()
    {
        m_file = SD.open(log_filename, FILE_WRITE);
    }

    void Logger::close()
    {
        if(m_file)
        {
            m_file.close();
        }
    }

    void Logger::print(const char* msg)
    {
        if(m_file)
        {
            m_file.print(msg);
        }
    }

    void Logger::print(int num)
    {
        if(m_file)
        {
            m_file.print(num);
        }
    }

    void Logger::println(const char* msg)
    {
        if(m_file)
        {
            m_file.println(msg);
        }
    }
}
