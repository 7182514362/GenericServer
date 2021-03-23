#ifndef SOCKET_SERVER
#define SOCKET_SERVER

#include "Log.h"

#include <netinet/in.h>

namespace generic
{
    class Socket
    {
        int m_fd;

    public:
        Socket(sa_family_t family)
        {

            m_fd = createSocketFd(family);
        }

        static int createSocketFd(sa_family_t family)
        {
            int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
            if (sockfd < 0)
            {
                LOG_FATAL << "socket(): " << strerror(errno);
                abort();
            }
            return sockfd;
        }

        void setReuseAddr(bool on)
        {
            int optval = on ? 1 : 0;
            ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR,
                         &optval, static_cast<socklen_t>(sizeof optval));
        }

        void setReusePort(bool on)
        {
            int optval = on ? 1 : 0;
            int ret = ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT,
                                   &optval, static_cast<socklen_t>(sizeof optval));
            if (ret < 0)
            {
                LOG_ERROR << "setsockopt(): " << strerror(errno);
            }
        }

        void setKeepAlive(bool on)
        {
            int optval = on ? 1 : 0;
            int ret = ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE,
                                   &optval, static_cast<socklen_t>(sizeof optval));
            if (ret < 0)
            {
                LOG_ERROR << "setsockopt(): " << strerror(errno);
            }
        }

        int fd() const
        {
            return m_fd;
        }
    };

    class SocketOP : Noncopyable
    {
    public:
        static int send(int fd, const void *buf, int count)
        {
            return ::send(fd, buf, count, 0);
        }

        static int recv(int fd, void *buf, int count)
        {
            return ::recv(fd, buf, count, 0);
        }

        static int send(int fd, BufferBase *buf)
        {
            int len = buf->data_size();
            if (len <= 0)
            {
                return -1;
            }

            LOG_TRACE << "SocketOP::send() " << buf->toString();

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

        static int recv(int fd, BufferBase *buf)
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

        static void close(int fd)
        {
            if (::close(fd) < 0)
            {
                LOG_ERROR << "::close() fd=" << fd << ": " << errno << strerror(errno);
            }
        }

        static int getSocketError(int sockfd)
        {
            int optval;
            socklen_t optlen = static_cast<socklen_t>(sizeof optval);

            if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
            {
                LOG_TRACE << "::getsockopt: " << strerror(errno);
                return errno;
            }
            else
            {
                return optval;
            }
        }
    };

}

#endif /*SOCKET_SERVER*/