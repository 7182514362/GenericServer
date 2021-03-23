#include "InetAddress.h"

using namespace generic;

InetAddress::InetAddress(int port, bool ipv6)
{
    if (ipv6)
    {
        memset(&m_addr6, 0, sizeof(m_addr6));
        m_addr6.sin6_family = AF_INET6;
        in6_addr ip = in6addr_any;
        m_addr6.sin6_addr = ip;
        m_addr6.sin6_port = ::htons(port);
    }
    else
    {
        memset(&m_addr4, 0, sizeof(m_addr4));
        m_addr4.sin_family = AF_INET;
        in_addr_t ip = INADDR_ANY;
        m_addr4.sin_addr.s_addr = ::htonl(ip);
        m_addr4.sin_port = ::htons(port);
    }
}

void InetAddress::setAddr(std::string ip, int port, bool ipv6)
{
    if (ipv6)
    {
        textToIP6(ip.c_str(), port);
    }
    else
    {
        textToIP4(ip.c_str(), port);
    }
}

std::string InetAddress::toString() const
{
    char buf[64];
    ipString(buf, sizeof(buf), (struct sockaddr *)(&m_addr6));
    int len = ::strlen(buf);
    int port = ::ntohs(m_addr4.sin_port);
    snprintf(buf + len, sizeof(buf) - len, ":%d", port);
    return buf;
}

void InetAddress::textToIP4(const char *ip, int port)
{
    memset(&m_addr4, 0, sizeof(m_addr4));
    m_addr4.sin_family = AF_INET;
    m_addr4.sin_port = htons(port);
    //
    int ret = ::inet_pton(AF_INET, ip, &m_addr4.sin_addr);
    if (ret = 0)
    {
        LOG_ERROR << "invalid ip string";
    }
    else if (ret < 0)
    {
        LOG_ERROR << "inet_pton" << strerror(errno);
    }
}

void InetAddress::textToIP6(const char *ip, int port)
{
    memset(&m_addr6, 0, sizeof(m_addr6));
    m_addr6.sin6_family = AF_INET6;
    m_addr6.sin6_port = ::htons(port);
    //
    int ret = ::inet_pton(AF_INET6, ip, &m_addr6.sin6_addr);
    if (ret = 0)
    {
        LOG_ERROR << "invalid ip string";
    }
    else if (ret < 0)
    {
        LOG_ERROR << "inet_pton" << strerror(errno);
    }
}

void InetAddress::ipString(char *buf, size_t size, const struct sockaddr *addr) const
{
    if (addr->sa_family == AF_INET)
    {
        assert(size >= INET_ADDRSTRLEN);
        const struct sockaddr_in *addr4 = (const struct sockaddr_in *)(addr);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    }
    else if (addr->sa_family == AF_INET6)
    {
        assert(size >= INET6_ADDRSTRLEN);
        const struct sockaddr_in6 *addr6 = (const struct sockaddr_in6 *)(addr);
        ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
    }
}