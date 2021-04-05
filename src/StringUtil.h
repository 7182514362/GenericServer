#ifndef STRING_UTIL_H
#define STRING_UTIL_H
#include <ostream>

namespace generic
{
    class SimpleString
    {
    public:
        const char *m_str;
        const int m_len;
        SimpleString() : m_str(nullptr), m_len(0) {}
        SimpleString(const char *str, int len) : m_str(str), m_len(len) {}
        SimpleString(const SimpleString &str) : m_str(str.m_str), m_len(str.m_len) {}
        friend std::ostream &operator<<(std::ostream &os, generic::SimpleString &str)
        {
            os << str.m_str;
            return os;
        }
    };
}

#endif