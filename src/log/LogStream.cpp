#include "LogStream.h"

#include <string.h>

using namespace generic;

#define MAX_LEN (1 << 10)

LogStream &LogStream::operator<<(const char c_)
{
    m_buffer.append(c_);
    return *this;
}
LogStream &LogStream::LogStream::operator<<(const char *str_)
{
    if (str_)
    {
        m_buffer.append(str_, strnlen(str_, MAX_LEN));
    }
    return *this;
}

LogStream &LogStream::operator<<(const std::string &str_)
{
    m_buffer.append(str_.c_str(), str_.length());
    return *this;
}

// integers only
#define INTEGER_TO_BUFFER(val_)           \
    std::string s = std::to_string(val_); \
    m_buffer.append(s.c_str(), s.length());

LogStream &LogStream::operator<<(int val_)
{
    INTEGER_TO_BUFFER(val_);
    return *this;
}
LogStream &LogStream::operator<<(unsigned int val_)
{
    INTEGER_TO_BUFFER(val_);
    return *this;
}

LogStream &LogStream::operator<<(long val_)
{
    INTEGER_TO_BUFFER(val_);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long val_)
{
    INTEGER_TO_BUFFER(val_);
    return *this;
}

LogStream &LogStream::operator<<(long long val_)
{
    INTEGER_TO_BUFFER(val_);
    return *this;
}
LogStream &LogStream::operator<<(unsigned long long val_)
{
    INTEGER_TO_BUFFER(val_);
    return *this;
}

LogStream &LogStream::operator<<(float val_)
{
    std::string s = std::to_string(val_);
    m_buffer.append(s.c_str(), s.length());
    return *this;
}
LogStream &LogStream::operator<<(double val_)
{
    std::string s = std::to_string(val_);
    m_buffer.append(s.c_str(), s.length());
    return *this;
}