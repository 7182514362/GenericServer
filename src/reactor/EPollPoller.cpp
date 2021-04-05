#include "EPollPoller.h"
#include "Log.h"
#include "Event.h"
#include "StringUtil.h"

#include <string.h>

using namespace generic;

EPollPoller::EPollPoller() : Poller(Poller::Type::EPOLL, createPollerFD()), m_events(16) {}

void EPollPoller::poll(int timeoutms, std::vector<Event *> &activeEvents)
{
    LOG_TRACE << "EPollPoller::poll";
    int num = ::epoll_wait(m_pollFD, &*m_events.begin(), m_events.size(), -1); //timeoutms
    if (num > 0)
    {

        for (int i = 0; i < num; ++i)
        {
            Event *p = static_cast<Event *>(m_events[i].data.ptr);
            p->set_revents(m_events[i].events);
            activeEvents.push_back(p);
        }
        if (num == m_events.size())
        {
            m_events.resize(2 * m_events.size());
        }
    }
    else if (num == 0)
    {
        LOG_DEBUG << "epoll_wait timeout";
    }
    else
    {
        switch (errno)
        {
        case EINTR:
            LOG_WARN << "epoll_wait interrupted";
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        default:
            LOG_FATAL << "EPollPoller::poll " << strerror(errno);
            abort();
        }
    }
}

void EPollPoller::registerEvent(const Event *e)
{
    updateEvent(e, EPOLL_CTL_ADD);
}

void EPollPoller::removeEvent(const Event *e)
{
    updateEvent(e, EPOLL_CTL_DEL);
}

void EPollPoller::modifyEvent(Event *e)
{
    updateEvent(e, EPOLL_CTL_MOD);
}

int EPollPoller::createPollerFD()
{
    int fd = ::epoll_create1(EPOLL_CLOEXEC);
    if (fd < 0)
    {
        LOG_FATAL << "::epoll_create1" << strerror(errno);
        abort();
    }
    return fd;
}

void EPollPoller::updateEvent(const Event *e, int op)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));

    event.events = e->events();
    event.data.ptr = const_cast<Event *>(e);
    int ret = ::epoll_ctl(m_pollFD, op, e->fd(), &event);
    if (ret < 0)
    {
        LOG_ERROR << "epoll_ctl: " << opstring(op)
                  << " fd=" << e->fd() << ": " << strerror(errno);
    }
}

SimpleString EPollPoller::opstring(int op)
{
    switch (op)
    {
    case EPOLL_CTL_ADD:
        return SimpleString("EPOLL_CTL_ADD", 13);
    case EPOLL_CTL_DEL:
        return SimpleString("EPOLL_CTL_DEL", 13);
    case EPOLL_CTL_MOD:
        return SimpleString("EPOLL_CTL_MOD", 13);
    default:
        return SimpleString("Unknown Operation", 17);
    }
}