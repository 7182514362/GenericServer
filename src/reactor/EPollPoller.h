#ifndef EPOLL_POLLER_H
#define EPOLL_POLLER_H

#include "Poller.h"
#include <sys/epoll.h>
#include <vector>

namespace generic
{
    class EPollPoller : public Poller
    {

    public:
        EPollPoller();

        void poll(int timeoutms, std::vector<Event *> &activeEvents) override;

        void registerEvent(const Event *e) override;

        void removeEvent(const Event *e) override;

        void modifyEvent(Event *e) override;

    private:
        int createPollerFD() override;

        void updateEvent(const Event *e, int op);

        SimpleString opstring(int op);

    private:
        std::vector<struct epoll_event> m_events;
    };
}

#endif