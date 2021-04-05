#include "Buffer.h"

#include <assert.h>
#include <unistd.h>
#include <string.h>

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

void BufferBase::bzero()
{
    clear();
    memset(m_data, 0, (int)(m_end - m_data));
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

#define MAX_SIZE (1 << 22)
char *Buffer::alloc(int size)
{
    if (size <= 0 || size >= MAX_SIZE)
    {
        return nullptr;
    }
    char *p = new char[size];
    if (!p)
    {
        perror("failed to create buffer");
        abort();
    }
    return p;
}