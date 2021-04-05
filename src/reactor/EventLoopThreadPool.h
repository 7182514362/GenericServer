#ifndef EVENTLOOP_THREAD_POOL_H
#define EVENTLOOP_THREAD_POOL_H

#include "EventLoopThread.h"

#include <thread>
#include <vector>
#include <memory>
#include <functional>


namespace generic
{
    class EventLoop;

    class EventLoopThreadPool
    {
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using EventLoopThreadUniquePtr = std::unique_ptr<EventLoopThread>;

    public:
        EventLoopThreadPool(int num = std::thread::hardware_concurrency());

        ~EventLoopThreadPool();

        void start();

        void stop();

        void join();

        EventLoopPtr &getNextLoop()
        {
            m_nextLoop = (m_nextLoop < m_loops.size() - 1) ? m_nextLoop + 1 : 0;
            return m_loops[m_nextLoop];
        }

        const EventLoopPtr &operator[](int num);

        std::vector<EventLoopPtr> &getAllLoop()
        {
            return m_loops;
        }

        int numOfThreads() const
        {
            return m_nthreads;
        }

    private:
        int m_nthreads;
        int m_nextLoop;

        std::vector<EventLoopThreadUniquePtr> m_threads;
        std::vector<EventLoopPtr> m_loops;
        //std::map<const int, EventLoopPtr> m_loopMap;
    };
}

#endif