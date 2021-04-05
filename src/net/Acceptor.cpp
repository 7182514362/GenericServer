#include "Acceptor.h"
#include "EventLoop.h"
#include "Log.h"
#include <assert.h>
#include <string.h>

using namespace generic;

Acceptor::Acceptor(EventLoopPtr loop, const InetAddress &listenAddr, NewConnCallback cb)
    : m_loop(loop),
      m_listenAddr(listenAddr),
      m_listenSocket(listenAddr.family()),
      m_connEvent(m_listenSocket.fd()),
      m_newConnCallback(cb)
{
    m_listenSocket.setReuseAddr(true);
    m_listenSocket.setReusePort(true);
    bind(listenAddr);

    m_connEvent.setReadCallback(std::bind(&Acceptor::accept, this));
    m_connEvent.enableRead();
    assert(m_loop);
    m_loop->addEvent(&m_connEvent);
}

void Acceptor::listen()
{
    int ret = ::listen(m_listenSocket.fd(), SOMAXCONN);
    if (ret < 0)
    {
        LOG_FATAL << "Acceptor::listen" << strerror(errno);
        abort();
    }
    LOG_INFO << "Acceptor::listen at " << m_listenAddr.toString();
}

void Acceptor::bind(const InetAddress &addr)
{
    int ret = ::bind(m_listenSocket.fd(), addr.getAddr(),
                     static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
    if (ret < 0)
    {
        LOG_FATAL << "::bind: " << strerror(errno);
        abort();
    }
}

void Acceptor::accept()
{
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof addr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof addr);
    int connfd = ::accept4(m_listenSocket.fd(), (struct sockaddr *)(&addr),
                           &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0)
    {
        LOG_ERROR << "::accept4: " << strerror(errno);
        return;
    }

    InetAddress peerAddr(addr);
    if (m_newConnCallback)
    {
        m_newConnCallback(connfd, peerAddr);
    }
}