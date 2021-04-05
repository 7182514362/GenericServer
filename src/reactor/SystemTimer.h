#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include "Noncopyable.h"
#include "Event.h"
#include <sys/timerfd.h>
#include <functional>

namespace generic
{

    class SysTimer : Noncopyable
    {

    public:
        SysTimer();

        int fd() const { return m_timerFd; }
        const Event *event() const { return &m_event; }
        void setInterval(int ms);
        void setExpireCallback(std::function<void()> cb) { m_expireCallback = std::move(cb); }

    private:
        void handleRead();
        void resetValue();
        void setTime(struct itimerspec *newValue, struct itimerspec *oldValue = nullptr);
        int createTimerFd();

    private:
        int m_timerFd;
        Event m_event;
        struct itimerspec m_value;
        // EventLoop *m_loop;
        std::function<void()> m_expireCallback;
    };
}

#endif