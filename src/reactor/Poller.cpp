#include "Poller.h"
#include "StringUtil.h"



using namespace generic;

std::map<Poller::Type, SimpleString> Poller::s_TypeString = {
    {Poller::Type::EPOLL, SimpleString("Epoll", 5)}};

SimpleString Poller::type()
{
    return s_TypeString[m_pollerType];
}

