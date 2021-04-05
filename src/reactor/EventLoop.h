#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "Noncopyable.h"
#include "Event.h"
#include "CurrentThread.h"

#include <mutex>
#include <memory>
#include <vector>
#include <functional>

namespace generic
{
    class TimerEntry;
    class SysTimer;
    class TimingWheel;
    class Poller;

    class EventLoop : Noncopyable
    {
        using TimerEntryPtr = std::shared_ptr<TimerEntry>;
        using EventPtr = std::shared_ptr<Event>;
        using SysTimerPtr = std::unique_ptr<SysTimer>;
        using TimerWheelPtr = std::unique_ptr<TimingWheel>;

    public:
        EventLoop();
        ~EventLoop();

        void loop();

        void runInLoop(std::function<void()> func);

        void quit() { m_quit = true; }

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
        int m_threadId;
        volatile bool m_quit;
        std::unique_ptr<Poller> m_poller;
        std::vector<Event *> m_activeEvents;

        Event m_wakeup;
        SysTimerPtr m_sysTimer;
        TimerWheelPtr m_timerWheel;

        std::mutex m_pendingMutex; // for m_pending
        std::vector<std::function<void()>> m_pending;
    };
}

#endif