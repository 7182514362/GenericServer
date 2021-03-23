#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include "Log.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#include <arpa/inet.h>
#include <stdint.h>
#include <endian.h>
#include <cstring>
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
        InetAddress() = default;
        InetAddress(int port, bool ipv6);

        InetAddress(std::string ip, int port, bool ipv6)
        {
            setAddr(ip, port, ipv6);
        }

        InetAddress(const sockaddr_in &addr) : m_addr4(addr) {}
        InetAddress(const sockaddr_in6 &addr) : m_addr6(addr) {}

        void setAddr(std::string ip, int port, bool ipv6);

        const struct sockaddr *getAddr() const
        {
            return (const struct sockaddr *)(&m_addr6);
        }

        sa_family_t family() const
        {
            return m_addr4.sin_family;
        }

        std::string toString() const;

    private:
        void textToIP4(const char *ip, int port);
        void textToIP6(const char *ip, int port);
        void ipString(char *buf, size_t size, const struct sockaddr *addr) const;
    };
}

#endif