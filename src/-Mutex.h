#ifndef MUTEX_H
#define MUTEX_H


#include "Util.h"
#include "Noncopyable.h"
#include <pthread.h>
#include <assert.h>


namespace generic
{

    class Mutex : Noncopyable
    {
        pthread_mutex_t m_mutex;

    public:
        Mutex()
        {
            ASSERT(pthread_mutex_init(&m_mutex, nullptr) == 0);
        }
        ~Mutex()
        {
            ASSERT(pthread_mutex_destroy(&m_mutex) == 0);
        }

        void lock()
        {
            ASSERT(pthread_mutex_lock(&m_mutex) == 0);
        }
        void unlock()
        {
            ASSERT(pthread_mutex_unlock(&m_mutex) == 0);
        }

        // only called by Condition
        pthread_mutex_t *getPthreadMutex()
        {
            return &m_mutex;
        }
    };

    class MutexLockGuard : Noncopyable
    {
    private:
        Mutex &m_mutex;

    public:
        MutexLockGuard(Mutex &m_) : m_mutex(m_)
        {
            m_mutex.lock();
        }
        ~MutexLockGuard(){
            m_mutex.unlock();
        }
    };
} // namespace generic
#endif //MUTEX_H