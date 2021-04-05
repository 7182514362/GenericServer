#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <memory>
#include <functional>

namespace generic
{

    class Event;
    class EventLoop;
    class InetAddress;

    class Connector
    {
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using ConnectErrorCallback = std::function<void()>;

    public:
        Connector(const std::shared_ptr<InetAddress>& addr, EventLoopPtr loop);
        ~Connector();

        void startConnect();
        void setSuccessCallback(std::function<void(int)> cb)
        {
            m_successCallback = std::move(cb);
        }

        void setFailedCallback(std::function<void()> cb)
        {
            m_failedCallback = std::move(cb);
        }

    private:
        void connect();
        // check if fd is writable
        void check(int fd);
        void retry(int fd);
        bool isSelfConnect(int fd);
        // run in loop thread
        void handleWrite();
        void handleError()
        {
        }

    private:
        const std::shared_ptr<InetAddress> m_serverAddr;
        EventLoopPtr m_loop;
        std::unique_ptr<Event> m_event;
        std::function<void(int)> m_successCallback;
        std::function<void()> m_failedCallback;
    };

}

#endif