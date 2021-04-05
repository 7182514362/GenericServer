#include "EventLoop.h"
#include "Poller.h"
#include "EPollPoller.h"
#include "Util.h"
#include "Log.h"
#include "SystemTimer.h"
#include "Timer.h"

#include <sys/eventfd.h>

using namespace generic;

#define TIMER_INTERVAL (1000)
EventLoop::EventLoop() : m_threadId(CurrentThread::tid()),
                         m_quit(false),
                         m_poller(new EPollPoller()),
                         m_activeEvents(),
                         m_wakeup(Event::createEventfd()),
                         m_sysTimer(new SysTimer()),
                         m_timerWheel(new TimingWheel(TIMER_INTERVAL)),
                         m_pendingMutex(),
                         m_pending()
{
    m_wakeup.setReadCallback(std::bind(&EventLoop::handleWakeup, this));
    m_wakeup.enableRead();
    m_poller->registerEvent(&m_wakeup);

    m_sysTimer->setInterval(TIMER_INTERVAL);
    m_sysTimer->setExpireCallback(std::bind(&TimingWheel::tick, m_timerWheel.get()));
    m_poller->registerEvent(m_sysTimer->event());
}

EventLoop::~EventLoop() {}

void EventLoop::loop()
{
    LOG_DEBUG << "EventLoop::loop started";
    while (!m_quit)
    {
        m_activeEvents.clear();
        m_poller->poll(100, m_activeEvents);
        LOG_DEBUG << "EventLoop::m_activeEvents " << m_activeEvents.size();
        for (auto &p : m_activeEvents)
        {
            p->handleEvent();
        }
        doPending();
    }
}

void EventLoop::doPending()
{
    LOG_DEBUG << "EventLoop::doPending size=" << m_pending.size();
    std::lock_guard<std::mutex> lock(m_pendingMutex);
    for (auto &f : m_pending)
    {
        f();
    }
    m_pending.clear();
}

void EventLoop::handleWakeup()
{
    uint64_t one = 1;
    ssize_t n = Util::read(m_wakeup.fd(), &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::handleWakeup failed";
    }
}

void EventLoop::wakeup()
{
    if (!isInLoopThread())
    {
        uint64_t one = 1;
        ssize_t n = Util::write(m_wakeup.fd(), &one, sizeof one);
        if (n != sizeof one)
        {
            LOG_ERROR << "EventLoop::wakeup failed ";
        }
    }
}

void EventLoop::runInLoop(std::function<void()> func)
{
    if (!isInLoopThread())
    {
        addPending(std::move(func));
        wakeup();
    }
    else
    {
        func();
    }
}

void EventLoop::addPending(std::function<void()> f)
{
    if (!isInLoopThread())
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        m_pending.emplace_back(f);
    }
    else
    {
        m_pending.emplace_back(f);
    }
}

void EventLoop::addEvent(Event *event)
{
    assert(event->isPolling() == false);
    if (!isInLoopThread())
    {
        addPending(
            [this, event]() {
                this->m_poller->registerEvent(event);
                event->setPolling(true);
            });
        wakeup();
    }
    else
    {
        m_poller->registerEvent(event);
        event->setPolling(true);
    }
}

void EventLoop::modifyEvent(Event *event)
{
    assert(event->isPolling() == true);
    if (!isInLoopThread())
    {
        addPending(
            [this, event]() {
                this->m_poller->modifyEvent(event);
            });
        wakeup();
    }
    else
    {
        m_poller->modifyEvent(event);
    }
}

void EventLoop::removeEvent(Event *event)
{
    if (!isInLoopThread())
    {
        addPending(
            [this, event]() {
                if (event->isPolling())
                {
                    this->m_poller->removeEvent(event);
                    event->setPolling(false);
                }
            });
        wakeup();
    }
    else
    {
        if (event->isPolling())
        {
            m_poller->removeEvent(event);
            event->setPolling(false);
        }
    }
}

void EventLoop::addTimer(std::shared_ptr<TimerEntry> &timer)
{
    if (!isInLoopThread())
    {
        addPending(std::bind(&TimingWheel::add, m_timerWheel.get(), timer));
        wakeup();
    }
    else
    {
        m_timerWheel->add(timer);
    }
}