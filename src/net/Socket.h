#ifndef SOCKET_SERVER
#define SOCKET_SERVER

#include <netinet/in.h>

namespace generic
{
    class BufferBase;

    class Socket
    {
        int m_fd;

    public:
        Socket(sa_family_t family) : m_fd(createSocketFd(family)) {}

        static int createSocketFd(sa_family_t family);

        void setReuseAddr(bool on);

        void setReusePort(bool on);

        void setKeepAlive(bool on);

        int fd() const
        {
            return m_fd;
        }
    };

    class SocketOP
    {
    private:
        SocketOP() {}

    public:
        static int send(int fd, const void *buf, int count)
        {
            return ::send(fd, buf, count, 0);
        }

        static int recv(int fd, void *buf, int count)
        {
            return ::recv(fd, buf, count, 0);
        }

        static int send(int fd, BufferBase *buf);

        static int recv(int fd, BufferBase *buf);

        static void close(int fd);

        static int getSocketError(int sockfd);

        static struct sockaddr_in6 getLocalAddr(int fd);

        static struct sockaddr_in6 getPeerAddr(int fd);
    };

}

#endif /*SOCKET_SERVER*/