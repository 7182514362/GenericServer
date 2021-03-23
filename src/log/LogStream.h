#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include "Noncopyable.h"
#include "Buffer.h"
#include "Util.h"

#include <memory>

namespace generic
{

#define LOGSTREAM_BUFFER_SIZE (1 << 8)

    class LogStream : Noncopyable
    {
        typedef FixedBuffer<LOGSTREAM_BUFFER_SIZE> LogStreamBuffer;
        LogStreamBuffer m_buffer;

    public:
        LogStream() : m_buffer() {}

        LogStream &operator<<(const char);
        LogStream &operator<<(const char *);
        LogStream &operator<<(StringWrapper &&str_)
        {
            m_buffer.append(str_.m_str, str_.m_len);
            return *this;
        }
        LogStream &operator<<(const std::string &);

        LogStream &operator<<(int);
        LogStream &operator<<(unsigned int);
        LogStream &operator<<(long);
        LogStream &operator<<(unsigned long);
        LogStream &operator<<(long long);
        LogStream &operator<<(unsigned long long);
        LogStream &operator<<(float);
        LogStream &operator<<(double);

        LogStreamBuffer *buffer()
        {
            return &m_buffer;
        }
    };
}
#endif