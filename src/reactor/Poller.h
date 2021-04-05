#ifndef POLLER_H
#define POLLER_H

#include "Noncopyable.h"
#include <map>
#include <vector>

namespace generic
{
    class Event;
    class SimpleString;

    class Poller : Noncopyable
    {

    public:
        enum class Type
        {
            EPOLL
        };

        static std::map<Type, SimpleString> s_TypeString;

        Poller(Type type, int fd) : m_pollFD(fd), m_pollerType(type)
        {
        }

        virtual void poll(int timeout, std::vector<Event *> &activeEvents) = 0;

        SimpleString type();

    public:
        virtual int createPollerFD() = 0;
        virtual void registerEvent(const Event *) = 0;
        virtual void removeEvent(const Event *e) = 0;
        virtual void modifyEvent(Event *e) = 0;

    protected:
        int m_pollFD;
        Type m_pollerType;
    };
}

#endif