#ifndef CONDITION_H
#define CONDITION_H

#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include "Util.h"
#include "Mutex.h"

namespace generic
{

    class Condition : Noncopyable
    {
    private:
        Mutex &m_mutex;
        pthread_cond_t m_cond;

    public:
        Condition(Mutex &m_) : m_mutex(m_)
        {
            ASSERT(pthread_cond_init(&m_cond, nullptr) == 0);
        }

        void wait()
        {
            ASSERT(pthread_cond_wait(&m_cond, m_mutex.getPthreadMutex()) == 0);
        }

        void timewait(int sec)
        {
            struct timeval now;
            struct timespec timeout;
            gettimeofday(&now, NULL);
            timeout.tv_sec = now.tv_sec + 1;
            timeout.tv_nsec = now.tv_usec * 1000;
            
            ASSERT(pthread_cond_timedwait(&m_cond, m_mutex.getPthreadMutex(), &timeout) == 0);
        }
        void notify()
        {
            ASSERT(pthread_cond_signal(&m_cond) == 0);
        }
        void notifyAll()
        {
            ASSERT(pthread_cond_broadcast(&m_cond) == 0);
        }
    };
} // namespace generic
#endif //CONDITION_H