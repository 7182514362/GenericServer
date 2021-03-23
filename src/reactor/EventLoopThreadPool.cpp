#include "EventLoopThreadPool.h"
using namespace generic;

EventLoopThreadPool::EventLoopThreadPool(int num)
    : m_nthreads(num),
      m_nextLoop(0),
      m_threads(),
      m_loops()
{
    assert(m_loops.empty() == true);
    for (int i = 0; i < num; ++i)
    {
        m_loops.push_back(std::make_shared<EventLoop>());
    }
    assert(m_threads.empty() == true);
    for (int i = 0; i < num; ++i)
    {
        m_threads.push_back(EventLoopThreadPtr(new EventLoopThread(m_loops[i])));
    }
}

void EventLoopThreadPool::start()
{
    LOG_TRACE << "EventLoopThreadPool::start";
    for (auto &t : m_threads)
    {
        t->start();
    }
    // wait until all thread started
    for (auto &t : m_threads)
    {
        while (!t->joinable())
        {
            usleep(1000);
        }
    }
}

void EventLoopThreadPool::stop()
{
    for (auto &thread : m_threads)
    {
        thread->stop();
    }
}

void EventLoopThreadPool::join()
{
    for (auto &thread : m_threads)
    {
        thread->join();
    }
}
