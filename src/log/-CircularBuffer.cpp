#include "CircularBuffer.h"

using namespace generic;

template <typename _BufferType, int BUFFER_NUM>
CircularBuffer<_BufferType, BUFFER_NUM>::CircularBuffer() : m_queue(),
                                                                    m_mutex(),
                                                                    m_not_empty(),
                                                                    m_not_full(),
                                                                    m_head(0),
                                                                    m_tail(0),
                                                                    m_stop(false)
{
    m_queue = static_cast<_BufferType *>(malloc(sizeof(_BufferType) * BUFFER_NUM));
    assert(m_queue != nullptr);
    for (int i = 0; i < BUFFER_NUM; ++i)
    {
        auto *p = new (m_queue + i) _BufferType();
        assert(p != nullptr);
    }
    m_head = 0;
    m_tail = 0;
}

template <typename _BufferType, int BUFFER_NUM>
CircularBuffer<_BufferType, BUFFER_NUM>::~CircularBuffer()
{
    for (int i = 0; i < BUFFER_NUM; ++i)
    {
        (m_queue + i)->~_BufferType();
    }

    free(m_queue);
}

template <typename _BufferType, int BUFFER_NUM>
void CircularBuffer<_BufferType, BUFFER_NUM>::push()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_stop && _full())
    {
        m_not_full.wait(lock);
    }
    if (m_stop)
    {
        return;
    }
    _inc(m_tail);
    m_not_empty.notify_one();
}

template <typename _BufferType, int BUFFER_NUM>
bool CircularBuffer<_BufferType, BUFFER_NUM>::push_nowait()
{
    LockGuard lock(m_mutex);
    if (_full())
    {
        return false;
    }
    _inc(m_tail);
    m_not_empty.notify_one();
    return true;
}

template <typename _BufferType, int BUFFER_NUM>
_BufferType *CircularBuffer<_BufferType, BUFFER_NUM>::pop()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_stop && _empty())
    {
        m_not_empty.wait(lock);
    }
    if (m_stop)
    {
        return nullptr;
    }
    _BufferType *buf = m_queue + m_head;
    _inc(m_head);
    m_not_full.notify_one();
    return buf;
}

template <typename _BufferType, int BUFFER_NUM>
_BufferType *CircularBuffer<_BufferType, BUFFER_NUM>::pop_nowait()
{
    LockGuard lock(m_mutex);
    return (m_queue + m_head);
}

template <typename _BufferType, int BUFFER_NUM>
bool CircularBuffer<_BufferType, BUFFER_NUM>::full() const
{
    LockGuard lock(m_mutex);
    return _full();
}

template <typename _BufferType, int BUFFER_NUM>
bool CircularBuffer<_BufferType, BUFFER_NUM>::empty() const
{
    LockGuard lock(m_mutex);
    return _empty();
}

template <typename _BufferType, int BUFFER_NUM>
_BufferType *CircularBuffer<_BufferType, BUFFER_NUM>::tail()
{
    LockGuard lock(m_mutex);
    return (m_queue + m_tail);
}

template <typename _BufferType, int BUFFER_NUM>
void CircularBuffer<_BufferType, BUFFER_NUM>::stop()
{
    LockGuard lock(m_mutex);
    m_stop = true;
    m_not_empty.notify_one();
    m_not_full.notify_one();
}
