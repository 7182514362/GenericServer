#ifndef EVENTLOOP_THREAD_H
#define EVENTLOOP_THREAD_H

#include "EventLoop.h"

#include <thread>
#include <functional>

namespace generic
{

    class EventLoopThread
    {
        using ThreadPtr = std::unique_ptr<std::thread>;
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using ThreadFunc = std::function<void()>;

    public:
        EventLoopThread(EventLoopPtr loop) : m_thread(), m_loop(loop) {}
        ~EventLoopThread()
        {
            stop();
            join();
        }

        void start(ThreadFunc pre = ThreadFunc(), ThreadFunc post = ThreadFunc());

        bool joinable()
        {
            return m_thread->joinable();
        }

        void join();

        void stop();

        EventLoopPtr &getLoop()
        {
            return m_loop;
        }

    private:
        void loop_thread(const ThreadFunc &pre, const ThreadFunc &post);

    private:
        ThreadPtr m_thread;
        EventLoopPtr m_loop;
    };
}

#endif