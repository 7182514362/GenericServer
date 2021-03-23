#include "RingBuffer.h"
#include <cstring>
#include <cstdio>

using namespace generic;

int RingBuffer::append(const char *data, int size)
{
    if (size <= 0)
    {
        return 0;
    }
    int nWritten = 0;
    int out = m_out;
    //printf("%d\n", size);
    if (m_in >= out)
    {
        int left = m_size - m_in;
        int len = std::min(size, left);
        memcpy(m_buf + m_in, data, len);
        m_in = (m_in + len) % m_size;
        nWritten += len;
        int remain = size - len;
        if (remain > 0)
        {
            int len2 = std::min(remain, out - 1);
            memcpy(m_buf + m_in, data + len, len2);
            m_in = (m_in + len2) % m_size;
            nWritten += len2;
        }
    }
    else
    {
        int len = std::min(size, out - m_in - 1);
        memcpy(m_buf + m_in, data, len);
        m_in += len;
        nWritten += len;
    }

    return nWritten;
}

int RingBuffer::retrive(char *buf, int size)
{
    if (size <= 0)
    {
        return 0;
    }
    int nRead = 0;
    int in = m_in;
    if (in >= m_out)
    {
        int len = std::min(size, in - m_out);
        memcpy(buf, m_buf + m_out, len);
        m_out += len;
        nRead = len;
    }
    else
    {
        int left = m_size - m_out;
        int len = std::min(size, left);
        memcpy(buf, m_buf + m_out, len);
        m_out = (m_out + len) % m_size;
        nRead += len;
        int remain = size - len;
        if (remain > 0)
        {
            int len2 = std::min(remain, in);
            memcpy(buf + len, m_buf + m_out, len2);
            m_out = (m_out + len2) % m_size;
            nRead += len2;
        }
    }
    return nRead;
}