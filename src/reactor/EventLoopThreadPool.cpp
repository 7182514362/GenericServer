#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "Log.h"

#include <assert.h>
#include <unistd.h>

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
        m_threads.push_back(EventLoopThreadUniquePtr(new EventLoopThread(m_loops[i])));
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    stop();
    join();
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

const std::shared_ptr<EventLoop> &EventLoopThreadPool::operator[](int num)
{
    if (num >= m_nthreads || num < 0)
    {
        return *m_loops.end();
    }
    return m_loops[num];
}