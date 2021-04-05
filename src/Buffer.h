#ifndef BUFFER_H
#define BUFFER_H

#include "Noncopyable.h"
#include "StringUtil.h"

#include <string>

namespace generic
{
    class BufferBase : Noncopyable
    {
    public:
        BufferBase(char *data, int size) : m_data(data),
                                           m_wptr(data),
                                           m_rptr(data),
                                           m_end(m_data + size)
        {
        }
        virtual ~BufferBase() = default;
        virtual int append(const char *buf, int len);
        virtual int append(const char c_);

        virtual int append(BufferBase &buf)
        {
            return append(buf.read_ptr(), buf.data_size());
        }

        virtual void append(std::string &str)
        {
            append(str.c_str(), str.length());
        }

        virtual const char *base() const
        {
            return m_data;
        }

        virtual char *write_ptr() const
        {
            return m_wptr;
        }

        virtual char *read_ptr() const
        {
            return m_rptr;
        }

        virtual int data_size() const
        {
            return (m_wptr - m_rptr);
        }

        virtual int total_size() const
        {
            return (m_end - m_data);
        }

        virtual bool empty() const
        {
            return (m_wptr == m_rptr);
        }

        virtual bool full() const
        {
            return (m_wptr == m_end);
        }

        virtual int remain_size() const
        {
            return (m_end - m_wptr);
        }

        virtual void clear()
        {
            m_wptr = m_data;
            m_rptr = m_data;
        }

        virtual void bzero();

        SimpleString toString() const
        {
            return SimpleString(m_rptr, data_size());
        }

        // from system kernel buffer
        virtual int read_in(int fd);
        // to system kernel buffer
        virtual int write_out(int fd);

    protected:
        char *m_data; // first byte

        char *m_wptr; // write pointer
        char *m_rptr; // read pointer
        char *m_end;  // the byte next to the last byte
    };

    template <int SIZE>
    class FixedBuffer : public BufferBase
    {
    private:
        char m_array[SIZE];

    public:
        FixedBuffer() : BufferBase(m_array, SIZE)
        {
        }

        ~FixedBuffer()
        {
        }
    };

    class Buffer : public BufferBase
    {
    public:
        Buffer(int size) : BufferBase(alloc(size), size)
        {
        }

    private:
        char *alloc(int size);
    };

} // namespace generic

#endif //BUFFER_H