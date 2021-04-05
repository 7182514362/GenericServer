#ifndef EVENT_H
#define EVENT_H

#include <functional>

namespace generic
{
    class Event
    {
        using EventHandler = std::function<void()>;

    public:
        Event(int fd);

        void setReadCallback(EventHandler cb) { m_readCallback = std::move(cb); }
        void setWriteCallback(EventHandler cb) { m_writeCallback = std::move(cb); }
        void setCloseCallback(EventHandler cb) { m_closeCallback = std::move(cb); }
        void setErrorCallback(EventHandler cb) { m_errorCallback = std::move(cb); }

        void handleEvent();

        int fd() const { return m_fd; }
        void close();
        int events() const { return m_events; }
        int revents() const { return m_revents; }
        void set_revents(int e) { m_revents = e; }
        void reset(int fd);

        void enableRead();

        void enableWrite(bool on);

        void enableET();

        bool isPolling() const
        {
            return m_polling;
        }

        void setPolling(bool b)
        {
            m_polling = b;
        }

        static int createEventfd();

    private:
        int m_fd;
        int m_events;   // Event types
        int m_revents;  // received events
        bool m_polling; // is event polling by poller?

        EventHandler m_readCallback;
        EventHandler m_writeCallback;
        EventHandler m_closeCallback;
        EventHandler m_errorCallback;
    };
} // namespace generic

#endif