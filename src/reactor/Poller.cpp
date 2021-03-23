#include "Poller.h"
#include <sys/epoll.h>
#include <errno.h>

using namespace generic;

std::map<Poller::Type, std::string> Poller::s_TypeString = {
    {Poller::Type::EPOLL, "Epoll"}};

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
            activeEvents.emplace_back(p);
        }
    }
    else if (num == 0)
    {
        LOG_DEBUG << "epoll_wait returned 0";
    }
    else
    {
        LOG_FATAL << "epoll_wait: " << strerror(errno);
        abort();
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

StringWrapper EPollPoller::opstring(int op)
{
    switch (op)
    {
    case EPOLL_CTL_ADD:
        return StringWrapper("EPOLL_CTL_ADD", 13);
    case EPOLL_CTL_DEL:
        return StringWrapper("EPOLL_CTL_DEL", 13);
    case EPOLL_CTL_MOD:
        return StringWrapper("EPOLL_CTL_MOD", 13);
    default:
        return StringWrapper("Unknown Operation", 17);
    }
}