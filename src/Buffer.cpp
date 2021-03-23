#include "Buffer.h"

using namespace generic;

int BufferBase::append(const char *buf, int len)
{
    assert(buf != nullptr);
    if (len <= 0)
    {
        return 0;
    }
    int n = remain_size();
    if (len <= n)
    {
        memcpy(m_wptr, buf, len);
        m_wptr += len;
        return len;
    }
    else
    {
        memcpy(m_wptr, buf, n);
        m_wptr = m_end;
        return n;
    }
}

int BufferBase::append(const char c_)
{
    if ((m_end - m_wptr) > 0)
    {
        *m_wptr = c_;
        ++m_wptr;
        return 1;
    }
    return 0;
}

// from system kernel buffer
int BufferBase::read_in(int fd)
{
    int n = ::read(fd, m_wptr, remain_size());
    if (n >= 0)
    {
        m_wptr += n;
    }
    else
    {
        //perror("Buffer::read_in");
    }
    return n;
}

// to system kernel buffer
int BufferBase::write_out(int fd)
{
    if (data_size() <= 0)
    {
        return 0;
    }
    int n = ::write(fd, m_rptr, data_size());
    if (n >= 0)
    {
        m_rptr += n;
        if (empty())
        {
            clear();
        }
    }
    return n;
}