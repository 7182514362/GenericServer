#include "BlockingQueue.h"

using namespace generic;

template <typename T>
void BlockingQueue<T>::put(const T &e)
{
    LockGuard lock(m_mutex);
    m_queue.push_back(e);
    m_cond.notify_one();
}

template <typename T>
void BlockingQueue<T>::put(T &&e)
{
    LockGuard lock(m_mutex);
    m_queue.push_back(std::move(e));
    m_cond.notify_one();
}

// blocked while empty
template <typename T>
T BlockingQueue<T>::take()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_queue.empty())
    {
        m_cond.wait(lock);
    }
    T t(std::move(m_queue.front()));
    m_queue.pop_front();
    return t;
}

template <typename T>
size_t BlockingQueue<T>::size() const
{
    LockGuard lock(m_mutex);
    return m_queue.size();
}
