#ifndef THREAD_H
#define THREAD_H

#include <functional>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>

#include "Noncopyable.h"
#include "Util.h"
#include "CurrentThread.h"

namespace generic
{
    

    class ThreadData
    {
        std::function<void()> m_func;
        pthread_t m_pthreadId;
        pid_t m_tid;
        std::string m_threadName;

    public:
        ThreadData(const std::function<void()> &func, const std::string &name) : m_func(func), m_pthreadId(0), m_tid(0), m_threadName(name)
        {
        }

        void run()
        {
            ::prctl(PR_SET_NAME, m_threadName.c_str());
            m_tid = CurrentThread::tid();
            m_func();
        }
    };

    class Thread : Noncopyable
    {
        //typedef void *(*pthreadFunc)(void *);
    private:
        bool m_started;
        bool m_joined;
        pid_t m_tid;
        pthread_t m_pthreadId;

        std::string m_threadName;
        std::function<void()> m_func;

        ThreadData *m_threadData;

    public:
        explicit Thread(std::function<void()> &&func, const std::string &name) : m_started(false),
                                                                                 m_joined(false),
                                                                                 m_tid(0),
                                                                                 m_pthreadId(0),
                                                                                 m_threadName(name),
                                                                                 m_func(func),
                                                                                 m_threadData(new ThreadData(m_func, m_threadName))
        {
        }

        ~Thread()
        {
            join();
            if (m_threadData)
                delete m_threadData;
        }

        static void *threadFunc(void *data)
        {
            ThreadData *threadData = static_cast<ThreadData *>(data);
            threadData->run();
            return nullptr;
        }

        void start()
        {
            if (!m_started)
            {
                int ret = pthread_create(&m_pthreadId, NULL, threadFunc, m_threadData);
                if (ret != 0)
                {
                    perror("Thread::start pthread_create");
                    abort();
                }
                m_started = true;
            }
        }

        void join()
        {
            if (!m_joined)
            {
                m_joined = true;
                int ret = pthread_join(m_pthreadId, NULL);
                if (ret != 0)
                {
                    perror("Thread::join pthread_join");
                    abort();
                }
            }
        }

        bool started() const { return m_started; }
        //pthread_t pthreadID() const { return m_pthreadid; }
        pid_t tid() const { return m_tid; }
        const std::string &name() const { return m_threadName; }
    };

} // namespace generic
#endif //THREAD_H