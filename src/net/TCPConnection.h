#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "Noncopyable.h"
#include "InetAddress.h"
#include "Event.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Socket.h"
#include "IdleConnections.h"
#include "Timer.h"

#include <mutex>
#include <vector>

namespace generic
{
    class TCPConnection : Noncopyable, public std::enable_shared_from_this<TCPConnection>
    {
#define BUFFER_SIZE (1 << 20)
    public:
        using BufferPtr = std::shared_ptr<Buffer>;
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
        using IdleConnectionsPtr = std::shared_ptr<IdleConnections>;

        using Callback = std::function<void(TCPConnectionPtr)>;

    public:
        TCPConnection(int fd, InetAddress &addr, EventLoopPtr loop)
            : m_peerAddr(addr),
              m_loop(loop),
              m_event(fd),
              m_rbuf(std::make_shared<Buffer>(BUFFER_SIZE)),
              m_wbuf(std::make_shared<Buffer>(BUFFER_SIZE)),
              m_entry(),
              m_timerList()
        {
            m_event.setReadCallback(std::bind(&TCPConnection::handleRead, this));
            m_event.setWriteCallback(std::bind(&TCPConnection::handleWrite, this));
            m_event.setCloseCallback(std::bind(&TCPConnection::handleClose, this));
            m_event.setErrorCallback(std::bind(&TCPConnection::handleError, this));

            m_event.enableRead();
        }

        ~TCPConnection()
        {
        }

        void init()
        {
            m_loop->runInLoop(
                [this]() {
                    if (m_connectedCallback)
                    {
                        m_connectedCallback(shared_from_this());
                    }
                    m_loop->addEvent(&m_event);
                });
        }

        void shutdown()
        {
            LOG_TRACE << "TCPConnection::shutdown";
            stopPollEvent();
            if (m_closeCallback)
                m_closeCallback(shared_from_this());
            SocketOP::close(getFd());
        }

        void startPollEvent()
        {
            m_loop->addEvent(&m_event);
        }

        void stopPollEvent()
        {
            m_loop->removeEvent(&m_event);
        }

        EventLoopPtr &getEventLoop()
        {
            return m_loop;
        }

        int getFd() const
        {
            return m_event.fd();
        }

        void setConnectedCallback(Callback cb)
        {
            m_connectedCallback = std::move(cb);
        }

        void setCloseCallback(Callback cb)
        {
            m_closeCallback = std::move(cb);
        }

        void setMsgCallback(Callback cb)
        {
            m_msgCallback = std::move(cb);
        }

        void setErrorCallback(Callback cb)
        {
            m_errorCallback = std::move(cb);
        }

        const InetAddress &getPeerAddr() const
        {
            return m_peerAddr;
        }

        void addTask(std::function<void()> f, int ms, bool repeat);

        void sendMsg(const char *msg, int len);

        std::weak_ptr<IdleConnectionEntry> getIdleEntry() const
        {
            return m_entry;
        }

        void setIdleEntry(std::weak_ptr<IdleConnectionEntry> ent)
        {
            m_entry = ent;
        }

        BufferPtr getWriteBuffer() const
        {
            return m_wbuf;
        }

        BufferPtr getReadBuffer() const
        {
            return m_rbuf;
        }

    private:
        // handler run in loop thread, no need to call addPending
        void handleRead();
        void handleWrite();
        void handleClose();
        void handleError();

        // called in m_loop's thread
        void addToLoop()
        {
            LOG_TRACE << "TCPConnection::addToLoop";
            m_loop->addEvent(&m_event);
        }

        /*
        // actively close conn
        void removeFromLoop()
        {
            LOG_TRACE << "TCPConnection::removeFromLoop";
            m_loop->removeEvent(&m_event);
        }

        void updateEvent()
        {
            m_loop->modifyEvent(&m_event);
        }
        */

    private:
        const InetAddress m_peerAddr;
        EventLoopPtr m_loop;
        Event m_event;

        std::mutex m_rlock;
        std::mutex m_wlock;
        BufferPtr m_rbuf;
        BufferPtr m_wbuf;

        Callback m_connectedCallback;
        Callback m_closeCallback;
        Callback m_msgCallback;
        Callback m_errorCallback;

        std::weak_ptr<IdleConnectionEntry> m_entry;
        // for both one-time and periodic timers,
        // using weak_ptr for better use of memory
        std::vector<std::weak_ptr<TimerEntry>> m_timerList;
    };

}

#endif