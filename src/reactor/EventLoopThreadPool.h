#ifndef EVENTLOOP_THREAD_POOL_H
#define EVENTLOOP_THREAD_POOL_H

#include "EventLoop.h"
#include "EventLoopThread.h"

#include <thread>
#include <vector>
#include <memory>
#include <functional>

// using std::thread
namespace generic
{
    class EventLoopThreadPool
    {
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using EventLoopThreadPtr = std::unique_ptr<EventLoopThread>;

    public:
        EventLoopThreadPool(int num = std::thread::hardware_concurrency());

        ~EventLoopThreadPool()
        {
            stop();
            join();
        }

        void start();

        void stop();

        void join();

        EventLoopPtr &getNextLoop()
        {
            m_nextLoop = (m_nextLoop < m_loops.size() - 1) ? m_nextLoop + 1 : 0;
            return m_loops[m_nextLoop];
        }

        EventLoopPtr &operator[](int num)
        {
            assert(num < m_nthreads && num >= 0);
            return m_loops[num];
        }

        std::vector<EventLoopPtr> &getAllLoop()
        {
            return m_loops;
        }

        /*
        void runInLoopThread(EventLoopPtr loop, std::function<void()> func)
        {
            loop->addPending(func);
            loop->wakeup();
        }

        void runInAllLoopThread(std::function<void()> func)
        {
            for (auto &loop : m_loops)
            {
                loop->addPending(func);
                loop->wakeup();
            }
        }
        */

        int numOfThreads() const
        {
            return m_nthreads;
        }

    private:
        int m_nthreads;
        int m_nextLoop;

        std::vector<EventLoopThreadPtr> m_threads;
        std::vector<EventLoopPtr> m_loops;
        //std::map<const int, EventLoopPtr> m_loopMap;
    };
}

#endif