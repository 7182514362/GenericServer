#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "Buffer.h"

#include <condition_variable>
#include <mutex>
#include <vector>
#include <cstring>

namespace generic
{
    template <typename _BufferType, int BUFFER_NUM>
    class CircularBuffer
    {
        using LockGuard = std::lock_guard<std::mutex>;

    private:
        _BufferType *m_queue;
        std::mutex m_mutex;
        std::condition_variable m_not_empty;
        std::condition_variable m_not_full;

        int m_head;
        int m_tail;
        // used to leave wait
        volatile bool m_stop;

    public:
        CircularBuffer();

        ~CircularBuffer();

        void push();

        bool push_nowait();

        _BufferType *pop();

        _BufferType *pop_nowait();

        bool full() const;

        bool empty() const;

        _BufferType *tail();

        void stop();

    private:
        // internal func without lock
        inline void _inc(int &idx)
        {
            ++idx;
            if (idx == BUFFER_NUM)
            {
                idx = 0;
            }
        }
        // internal func without lock
        inline bool _full()
        {
            int i = m_tail;
            _inc(i);
            return (i == m_head);
        }

        inline bool _empty()
        {
            return (m_head == m_tail);
        }
    };
}

#endif