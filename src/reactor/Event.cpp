#include "Event.h"
#include "Log.h"

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace generic;

Event::Event(int fd) : m_fd(fd), m_events(0), m_revents(0), m_polling(false) {}

void Event::enableRead()
{
    m_events |= (EPOLLIN | EPOLLPRI);
}

void Event::enableWrite(bool on)
{
    m_events = (on) ? (m_events | EPOLLOUT) : (m_events & (~EPOLLOUT));
}

void Event::enableET()
{
    m_events |= EPOLLET;
}

void Event::handleEvent()
{
    LOG_TRACE << "Event::handleEvent";
    if ((m_revents & EPOLLHUP) && !(m_revents & EPOLLIN))
    {
        LOG_TRACE << "Event::handleEvent EPOLLHUP";
        if (m_closeCallback)
            m_closeCallback();
    }

    if (m_revents & EPOLLERR)
    {
        LOG_ERROR << "handleEvent EPOLLERR";
        if (m_errorCallback)
            m_errorCallback();
    }

    // Stream socket peer closed connection, or shut down writing half of connection
    if (m_revents & EPOLLRDHUP)
    {
        LOG_TRACE << "Event::handleEvent EPOLLRDHUP";
        if (m_closeCallback)
            m_closeCallback();
    }

    if (m_revents & (EPOLLIN | EPOLLPRI))
    {
        LOG_TRACE << "Event::handleEvent EPOLLIN";
        if (m_readCallback)
            m_readCallback();
    }

    if (m_revents & EPOLLOUT)
    {
        LOG_TRACE << "Event::handleEvent EPOLLOUT";
        if (m_writeCallback)
            m_writeCallback();
    }
    // reset revents
    m_revents = 0;
}

int Event::createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_ERROR << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

void Event::reset(int fd)
{
    m_fd = fd;
    m_events = 0;
    m_revents = 0;
    m_readCallback = nullptr;
    m_writeCallback = nullptr;
    m_closeCallback = nullptr;
    m_errorCallback = nullptr;
}

void Event::close()
{
    ::close(m_fd);
    m_fd = -1;
}