#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "TCPServer.h"
#include <memory>

namespace generic
{

    class HttpServer
    {
    public:
    private:
        std::shared_ptr<TCPServer> m_server;
    };
}
#endif