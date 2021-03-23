#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "Event.h"
#include "Poller.h"
#include "Log.h"
#include "SystemTimer.h"
#include "Timer.h"
#include "Noncopyable.h"
#include "CurrentThread.h"

#include <mutex>
#include <memory>
#include <vector>
#include <atomic>
#include <sys/eventfd.h>

namespace generic
{

    class EventLoop : Noncopyable
    {
#define TIMER_INTERVAL (1000)
        using TimerEntryPtr = std::shared_ptr<TimerEntry>;
        using EventPtr = std::shared_ptr<Event>;
        using SysTimerPtr = std::unique_ptr<SysTimer>;
        using TimerWheelPtr = std::unique_ptr<TimingWheel>;

        enum State
        {
            STOPPED = 0,
            POLLING,
            HANDLING,  // handling events
            DOPENDING, // doing pending functors
        };

    public:
        EventLoop() : m_poller(new EPollPoller()),
                      m_activeEvents(),
                      m_quit(false),
                      m_threadId(CurrentThread::tid()),
                      m_wakeup(Event::createEventfd()),
                      m_sysTimer(new SysTimer()),
                      m_timerWheel(new TimingWheel(TIMER_INTERVAL)),
                      m_pendingMutex(),
                      m_state(STOPPED),
                      m_pending()
        {
            m_wakeup.setReadCallback(std::bind(&EventLoop::handleWakeup, this));
            m_wakeup.enableRead();
            m_poller->registerEvent(&m_wakeup);

            m_sysTimer->setInterval(TIMER_INTERVAL);
            m_sysTimer->setExpireCallback(std::bind(&TimingWheel::tick, m_timerWheel.get()));
            m_poller->registerEvent(m_sysTimer->event());
        }

        ~EventLoop() = default;

        void loop();

        void runInLoop(std::function<void()> func);

        void quit()
        {
            m_quit = true;
        }

        void wakeup();

        void addPending(std::function<void()> f);
        // these functions check if current thread is the loop thread,
        // if not, add to loop thread's pending functors and then wake up loop thread.
        void addEvent(Event *event);
        void modifyEvent(Event *event);
        void removeEvent(Event *event);
        void addTimer(std::shared_ptr<TimerEntry> &timer);

        const int getThreadId() const
        {
            return m_threadId;
        }

        void setThreadId(int tid)
        {
            m_threadId = tid;
        }

    private:
        void doPending();
        void handleWakeup();

        bool isInLoopThread() const
        {
            return (m_threadId == CurrentThread::tid());
        }

    private:
        std::unique_ptr<Poller> m_poller;
        std::vector<Event *> m_activeEvents;
        volatile bool m_quit;
        int m_threadId;

        Event m_wakeup;
        SysTimerPtr m_sysTimer;
        TimerWheelPtr m_timerWheel;

        std::mutex m_pendingMutex; // for m_pending
        std::atomic<int> m_state;

        std::vector<std::function<void()>> m_pending;
    };
}

#endif