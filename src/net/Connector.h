#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <memory>
#include <functional>

#include "Log.h"
#include "Socket.h"
#include "Event.h"
#include "EventLoop.h"
#include "InetAddress.h"

namespace generic
{

    class Connector
    {
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using ConnectErrorCallback = std::function<void()>;
        using NewConnectionCallback = std::function<void(int)>;

    public:
        Connector(InetAddress &addr, EventLoopPtr loop) : m_serverAddr(addr),
                                                          m_loop(loop),
                                                          m_event()

        {
        }

        void connect()
        {
            int sockfd = Socket::createSocketFd(m_serverAddr.family());
            // Nonblocking
            connect(sockfd, m_serverAddr.getAddr());
        }

        void setNewConnectionCallback(NewConnectionCallback cb)
        {
            m_newConnCallback = std::move(cb);
        }

    private:
        void connect(int fd, const sockaddr *addr)
        {
            LOG_INFO << "Connecting to " << m_serverAddr.toString();

            int ret = ::connect(fd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
            int errorcode = (ret == 0) ? 0 : errno;
            switch (errorcode)
            {
            case 0:
                LOG_TRACE << "Connector::connect(): connected";
                connecting(fd);
                break;
            case EINPROGRESS:
            case EINTR:
            case EISCONN:
                LOG_TRACE << "Connector::connect(): " << strerror(errno);
                connecting(fd);
                break;

            case EAGAIN:
            case EADDRINUSE:
            case EADDRNOTAVAIL:
            case ECONNREFUSED:
            case ENETUNREACH:
                LOG_TRACE << "Connector::connect(): " << strerror(errno);
                retry(fd);
                break;

            case EACCES:
            case EPERM:
            case EAFNOSUPPORT:
            case EALREADY:
            case EBADF:
            case EFAULT:
            case ENOTSOCK:
                LOG_ERROR << "Connector::connect(): " << strerror(errno);
                retry(fd);
                break;

            default:
                LOG_ERROR << "Connector::connect() Unkown error " << errorcode;
                retry(fd);
                break;
            }
        }

        // check if fd is writable
        void connecting(int fd)
        {
            m_event.reset(new Event(fd));
            m_event->setWriteCallback(std::bind(&Connector::handleWrite, this));
            m_event->setErrorCallback(std::bind(&Connector::handleError, this));
            m_event->enableWrite(true);
            m_loop->addEvent(m_event.get());
        }

        void retry(int fd)
        {
            SocketOP::close(fd);
        }

        // run in loop thread
        void handleWrite()
        {
            LOG_TRACE << "Connector::handleWrite() ";
            m_loop->removeEvent(m_event.get());
            int fd = m_event->fd();
            int errorcode = SocketOP::getSocketError(fd);
            if (errorcode <= 0)
            {
                LOG_TRACE << "Connection established";
                if (m_newConnCallback)
                {
                    m_newConnCallback(fd);
                }
            }
            else if (false) // check if self connect
            {
            }
            else
            {
                LOG_ERROR << "Connect failed, " << strerror(errorcode);
                SocketOP::close(fd);
            }
            m_event->enableWrite(false);
        }

        void handleError()
        {
        }

    private:
        InetAddress m_serverAddr;
        EventLoopPtr m_loop;
        std::unique_ptr<Event> m_event;
        NewConnectionCallback m_newConnCallback;
    };

}

#endif