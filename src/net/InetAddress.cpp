#include "InetAddress.h"
#include "Socket.h"
#include "Log.h"

#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

//#include <stdint.h>
//#include <endian.h>
//#include <cstring>

using namespace generic;

InetAddress::InetAddress(const struct sockaddr_in6 &addr)
{
    memset(&m_addr6, 0, sizeof(m_addr6));
    if (addr.sin6_family == AF_INET6)
    {
        memcpy(&m_addr6, &addr, sizeof(struct sockaddr_in6));
    }
    else if (addr.sin6_family == AF_INET)
    {
        memcpy(&m_addr4, &addr, sizeof(struct sockaddr_in));
    }
}

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

void InetAddress::setAddr(const std::string &ip, int port, bool ipv6)
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

bool InetAddress::operator==(const InetAddress &addr) const
{
    bool b;
    if (m_addr6.sin6_family == AF_INET)
    {
        b = (m_addr4.sin_family == addr.m_addr4.sin_family) &&
            (m_addr4.sin_port == addr.m_addr4.sin_port) &&
            (m_addr4.sin_addr.s_addr == addr.m_addr4.sin_addr.s_addr);
    }
    else
    {
        b = (m_addr6.sin6_family == addr.m_addr6.sin6_family) &&
            (m_addr6.sin6_port == addr.m_addr6.sin6_port) &&
            (memcmp(&m_addr6.sin6_addr, &addr.m_addr6.sin6_addr, sizeof(struct in6_addr)));
    }

    return b;
}

size_t InetAddress::hash() const
{
    int addr_len =
        (m_addr6.sin6_family == AF_INET) ? sizeof(m_addr4.sin_addr) : sizeof(m_addr6.sin6_addr);
    unsigned long *p =
        (m_addr6.sin6_family == AF_INET) ? (unsigned long *)&m_addr4.sin_addr.s_addr : (unsigned long *)&m_addr6.sin6_addr;
    size_t h = 0xFFFFFFFF;
    for (int i = 0; i < addr_len / sizeof(int); ++i)
    {
        h ^ std::hash<int>()(p[i]);
    }
    h + m_addr6.sin6_port;
    return h;
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
        LOG_ERROR << "inet_pton: " << strerror(errno);
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