#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Event.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

namespace generic
{

    class Acceptor
    {
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        EventLoopPtr m_loop;
        InetAddress m_listenAddr;
        Socket m_listenSocket;
        Event m_connEvent;

        using NewConnCallback = std::function<void(int, InetAddress &)>;

        NewConnCallback m_newConnCallback;

    public:
        Acceptor(EventLoopPtr loop, const InetAddress &listenAddr, NewConnCallback cb);

        void listen();

    private:
        void bind(const InetAddress &addr);

        void accept();
    };
} // namespace generic

#endif