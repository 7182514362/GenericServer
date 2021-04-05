#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <netinet/in.h>
#include <string>

namespace generic
{

    class InetAddress
    {
        union
        {
            struct sockaddr_in m_addr4;
            struct sockaddr_in6 m_addr6;
        };

    public:
        InetAddress(const struct sockaddr_in6 &addr);
        
        InetAddress(int port, bool ipv6);

        InetAddress(const std::string& ip, int port, bool ipv6)
        {
            setAddr(ip, port, ipv6);
        }

        void setAddr(const std::string& ip, int port, bool ipv6);

        const struct sockaddr *getAddr() const
        {
            return (const struct sockaddr *)(&m_addr6);
        }

        bool isIpv6() const
        {
            return m_addr6.sin6_family == AF_INET6;
        }

        bool operator==(const InetAddress &addr) const;

        sa_family_t family() const
        {
            return m_addr4.sin_family;
        }

        std::string toString() const;

        size_t hash() const;

    private:
        void textToIP4(const char *ip, int port);
        void textToIP6(const char *ip, int port);
        void ipString(char *buf, size_t size, const struct sockaddr *addr) const;
    };
}

#endif