#include "Socket.h"
#include "Log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

using namespace generic;

int Socket::createSocketFd(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOG_FATAL << "socket: " << strerror(errno);
        abort();
    }
    return sockfd;
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR,
                 &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT,
                           &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0)
    {
        LOG_ERROR << "setsockopt: " << strerror(errno);
    }
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE,
                           &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0)
    {
        LOG_ERROR << "setsockopt: " << strerror(errno);
    }
}

int SocketOP::send(int fd, BufferBase *buf)
{
    int len = buf->data_size();
    if (len <= 0)
    {
        return -1;
    }

    LOG_TRACE << "SocketOP::send " << buf->toString();

    int n = buf->write_out(fd);
    if (n > 0)
    {
        LOG_TRACE << "SocketOP::send " << n << " byte(s)";
    }
    else if (n < 0)
    {
        switch (errno)
        {
        case EAGAIN: // no data sent, wait for EPOLLOUT
            break;
        default:
            LOG_ERROR << "SocketOP::send errno=" << errno << strerror(errno);
            break;
        }
    }
    else // when will this happen?
    {
        LOG_ERROR << "SocketOP::send 0 bytes";
    }
    return n;
}

int SocketOP::recv(int fd, BufferBase *buf)
{
    LOG_TRACE << "SocketOP::recv";
    // check if it's read all or buf is full
    int n = buf->read_in(fd);
    if (n > 0)
    {
        LOG_TRACE << "SocketOP::recv" << n << " byte(s)";
    }
    else if (n < 0)
    {
        switch (errno)
        {
        case ECONNRESET: // leave it to caller
        case EAGAIN:     // no data to read
            break;
        default:
            LOG_ERROR << "SocketOP::recv errno=" << errno << strerror(errno);
            break;
        }
    }
    else // n==0, leave it to caller
    {
    }

    return n;
}

void SocketOP::close(int fd)
{
    if (::close(fd) < 0)
    {
        LOG_ERROR << "::close fd=" << fd << ": " << errno << strerror(errno);
    }
}

int SocketOP::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    // SO_ERROR: default = 0
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        LOG_TRACE << "::getsockopt: " << strerror(errno);
        return -1;
    }
    else
    {
        return optval;
    }
}

struct sockaddr_in6 SocketOP::getLocalAddr(int fd)
{
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof addr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
    if (::getsockname(fd, (struct sockaddr *)(&addr), &addrlen) < 0)
    {
        LOG_ERROR << "sockets::getPeerAddr";
    }
    return addr;
}

struct sockaddr_in6 SocketOP::getPeerAddr(int fd)
{
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof addr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
    if (::getpeername(fd, (struct sockaddr *)(&addr), &addrlen) < 0)
    {
        LOG_ERROR << "sockets::getPeerAddr";
    }
    return addr;
}