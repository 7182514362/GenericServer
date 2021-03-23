#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "Buffer.h"
#include <atomic>

namespace generic
{
    class RingBuffer
    {
    public:
        RingBuffer(int size) : m_size(size),
                               m_buf(new char[size]),
                               m_in(0),
                               m_out(0) {}

        int append(const char *data, int size);

        int retrive(char *buf, int size);

        char *writePtr()
        {
            return m_buf + m_in;
        }

        char *readPtr()
        {
            return m_buf + m_out;
        }

        bool full() const
        {
            return (m_in + 1) == m_out;
        }

        bool empty() const
        {
            return m_in == m_out;
        }

        int data_size() const
        {
            int in = m_in;
            int out = m_out;
            return (in >= out) ? in - out : m_size - out + in;
        }

        int avail_size() const
        {
            int in = m_in;
            int out = m_out;
            return (in >= out) ? out - 1 + m_size - in : out - in - 1;
        }

    private:
        const int m_size;
        char *m_buf;
        std::atomic<int> m_in;
        std::atomic<int> m_out;
    };

}

#endif