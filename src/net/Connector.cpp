#include "Connector.h"

#include "Log.h"
#include "Socket.h"
#include "Event.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Timer.h"

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace generic;

Connector::Connector(const std::shared_ptr<InetAddress> &addr, EventLoopPtr loop)
    : m_serverAddr(addr),
      m_loop(loop),
      m_event()

{
}
Connector::~Connector() {}

void Connector::startConnect()
{
    m_loop->runInLoop(std::bind(&Connector::connect, this));
}

void Connector::connect()
{
    int fd = Socket::createSocketFd(m_serverAddr->family());
    const sockaddr *addr = m_serverAddr->getAddr();

    LOG_INFO << "Connecting to " << m_serverAddr->toString();
    int ret = ::connect(fd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    int err = (ret == 0) ? 0 : errno;
    switch (err)
    {
    case 0:
        LOG_TRACE << "Connector::connect: connected";
        check(fd);
        break;
    case EINTR:
    case EISCONN:
    case EALREADY:
    case EINPROGRESS:
        check(fd);
        break;
    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ENETUNREACH:
    case ECONNREFUSED:
        LOG_TRACE << "Connector::connect: " << strerror(errno);
        retry(fd);
        break;
    case EPERM:
    case EACCES:
    case EAFNOSUPPORT:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        LOG_FATAL << "Connector::connect: " << strerror(errno);
        if (m_failedCallback)
        {
            m_failedCallback();
        }
        break;

    default:
        LOG_ERROR << "Connector::connect Unkown error " << err;
        retry(fd);
        break;
    }
}

// check if fd is writable
void Connector::check(int fd)
{
    m_event.reset(new Event(fd));
    m_event->setWriteCallback(std::bind(&Connector::handleWrite, this));
    m_event->setErrorCallback(std::bind(&Connector::handleError, this));
    m_event->enableWrite(true);
    m_loop->addEvent(m_event.get());
}

void Connector::retry(int fd)
{
    SocketOP::close(fd);
    static int retry_times = 0;
    ++retry_times;
    if (retry_times < 3)
    {
        std::shared_ptr<TimerEntry> timer =
            std::make_shared<TimerEntry>(std::bind(&Connector::connect, this), 5, false);
        m_loop->addTimer(timer);
    }
    else
    {
        if (m_failedCallback)
        {
            m_failedCallback();
        }
    }
}

bool Connector::isSelfConnect(int fd)
{
    struct sockaddr_in6 local = SocketOP::getLocalAddr(fd);
    struct sockaddr_in6 peer = SocketOP::getPeerAddr(fd);
    if (local.sin6_family == AF_INET)
    {
        const struct sockaddr_in *laddr4 = (struct sockaddr_in *)(&local);
        const struct sockaddr_in *raddr4 = (struct sockaddr_in *)(&peer);
        return laddr4->sin_port == raddr4->sin_port && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    }
    else if (local.sin6_family == AF_INET6)
    {
        return local.sin6_port == peer.sin6_port && memcmp(&local.sin6_addr, &peer.sin6_addr, sizeof local.sin6_addr) == 0;
    }
    else
    {
        return false;
    }
}

// run in loop thread
void Connector::handleWrite()
{
    LOG_TRACE << "Connector::handleWrite";
    m_loop->removeEvent(m_event.get());
    int fd = m_event->fd();
    int err = SocketOP::getSocketError(fd);
    if (err == 0)
    {
        LOG_TRACE << "Connection established";
        if (m_successCallback)
        {
            m_successCallback(fd);
        }
    }
    else if (isSelfConnect(fd)) // check if self connect
    {
        LOG_ERROR << "Connect failed, " << strerror(err);
        retry(fd);
    }
    else // err>0 || err<0
    {
        LOG_ERROR << "Connect failed, " << strerror(err);
        SocketOP::close(fd);
        if (m_failedCallback)
        {
            m_failedCallback();
        }
    }
    m_event->enableWrite(false);
}