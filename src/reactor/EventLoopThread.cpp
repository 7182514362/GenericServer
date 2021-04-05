#include "EventLoopThread.h"
#include "EventLoop.h"
#include "CurrentThread.h"

#include <assert.h>

using namespace generic;

void EventLoopThread::start(std::function<void()> pre, std::function<void()> post)
{
    assert(m_thread.get() == nullptr);
    m_thread.reset(new std::thread(
        std::bind(&EventLoopThread::loop_thread, this, pre, post)));
}

void EventLoopThread::join()
{
    if (m_thread && m_thread->joinable())
    {
        m_thread->join();
        m_thread = nullptr;
    }
}

void EventLoopThread::stop()
{
    m_loop->quit();
}

void EventLoopThread::loop_thread(const ThreadFunc &pre, const ThreadFunc &post)
{
    assert(m_loop != nullptr);
    m_loop->setThreadId(CurrentThread::tid());

    if (pre)
    {
        pre();
    }

    m_loop->loop();

    if (post)
    {
        post();
    }
}