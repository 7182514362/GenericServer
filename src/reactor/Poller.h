#ifndef POLLER_H
#define POLLER_H

#include "Noncopyable.h"
#include "Log.h"
#include "Event.h"

#include <vector>

namespace generic
{

    class Poller : Noncopyable
    {

    public:
        enum class Type
        {
            EPOLL
        };

        static std::map<Type, std::string> s_TypeString;

        Poller(Type type, int fd) : m_pollFD(fd), m_pollerType(type)
        {
        }

        virtual void poll(int timeout, std::vector<Event *> &activeEvents) = 0;

        std::string type()
        {
            return s_TypeString[m_pollerType];
        }

    public:
        virtual int createPollerFD() = 0;
        virtual void registerEvent(const Event *) = 0;
        virtual void removeEvent(const Event *e) = 0;
        virtual void modifyEvent(Event *e) = 0;

    protected:
        int m_pollFD;
        Type m_pollerType;
    };

    class EPollPoller : public Poller
    {

    public:
        EPollPoller() : Poller(Poller::Type::EPOLL, createPollerFD()), m_events(16) {}

        void poll(int timeoutms, std::vector<Event *> &activeEvents) override;

        void registerEvent(const Event *e) override;

        void removeEvent(const Event *e) override;

        void modifyEvent(Event *e) override;

    private:
        int createPollerFD() override;

        void updateEvent(const Event *e, int op);

        StringWrapper opstring(int op);

    private:
        std::vector<struct epoll_event> m_events;
    };
}

#endif