#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Event.h"
#include "InetAddress.h"
#include "Socket.h"
#include <memory>
#include <functional>

namespace generic
{
    class EventLoop;

    class Acceptor
    {
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using NewConnCallback = std::function<void(int, InetAddress &)>;

    public:
        Acceptor(EventLoopPtr loop, const InetAddress &listenAddr, NewConnCallback cb);

        void listen();

    private:
        void bind(const InetAddress &addr);

        void accept();

    private:
        EventLoopPtr m_loop;
        InetAddress m_listenAddr;
        Socket m_listenSocket;
        Event m_connEvent;
        NewConnCallback m_newConnCallback;
    };
} // namespace generic

#endif