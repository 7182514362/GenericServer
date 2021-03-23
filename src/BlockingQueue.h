#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include "Noncopyable.h"

#include <condition_variable>
#include <mutex>
#include <deque>

namespace generic
{
    template <typename T>
    class BlockingQueue : Noncopyable
    {
        using LockGuard = std::lock_guard<std::mutex>;

    private:
        mutable std::mutex m_mutex;
        // not empty
        std::condition_variable m_cond;
        std::deque<T> m_queue;

    public:
        BlockingQueue() : m_mutex(), m_cond(), m_queue()
        {
        }

        void put(const T &e);
        void put(T &&e);

        // blocked while empty
        T take();

        size_t size() const;
    };

}

#endif