#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "Noncopyable.h"
#include "InetAddress.h"
#include "Event.h"
//#include "EventLoop.h"
//#include "Buffer.h"
//#include "Socket.h"
//#include "IdleConnections.h"
//#include "Timer.h"

#include <mutex>
#include <vector>
#include <memory>
#include <functional>


namespace generic
{

    class IdleConnections;
    class IdleConnectionEntry;
    class EventLoop;
    class Buffer;
    class SocketOP;
    class TimerEntry;

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
        TCPConnection(int fd, InetAddress &addr, EventLoopPtr loop);

        ~TCPConnection()
        {
        }

        void init();

        void shutdown();

        void startPollEvent();

        void stopPollEvent();

        EventLoopPtr &getEventLoop()
        {
            return m_loop;
        }

        int getFd() const;

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
            return m_peer;
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
        void addToLoop();

        bool operator==(const TCPConnection &conn) const;

        static size_t connection_hash(const TCPConnection &conn);

    private:
        const InetAddress m_local;
        const InetAddress m_peer;
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