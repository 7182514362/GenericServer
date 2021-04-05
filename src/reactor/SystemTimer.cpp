#include "SystemTimer.h"
#include "Event.h"
#include "Util.h"
#include "Log.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include <string.h>

using namespace generic;

SysTimer::SysTimer() : m_timerFd(createTimerFd()), m_event(m_timerFd)
{
    resetValue();
    m_event.setReadCallback(std::bind(&SysTimer::handleRead, this));
    m_event.enableRead();
}

void SysTimer::setInterval(int ms)
{
    resetValue();

    int sec = ms / 1000;
    ms = ms % 1000;
    m_value.it_interval.tv_sec = sec;
    m_value.it_interval.tv_nsec = ms * 1000000;
    m_value.it_value.tv_sec = sec;
    m_value.it_value.tv_nsec = ms * 1000000;
    setTime(&m_value);
}

void SysTimer::handleRead()
{
    uint64_t exp = 0;
    Util::read(m_timerFd, &exp, sizeof exp);
    if (m_expireCallback)
    {
        m_expireCallback();
    }
}

void SysTimer::resetValue()
{
    memset(&m_value, 0, sizeof(struct itimerspec));
}

void SysTimer::setTime(struct itimerspec *newValue, struct itimerspec *oldValue)
{
    int ret = timerfd_settime(m_timerFd, 0, newValue, oldValue);
    if (ret < 0)
    {
        LOG_FATAL << "timerfd_settime: " << strerror(errno);
        close(m_timerFd);
        abort();
    }
}

int SysTimer::createTimerFd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        perror("timerfd_create");
        abort();
    }
    return timerfd;
}