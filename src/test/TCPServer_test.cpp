#include <iostream>

#include "Util.h"
#include "Log.h"
#include "InetAddress.h"
#include "TCPConnection.h"
#include "TCPServer.h"
#include "Config.h"

using namespace std;

//std::shared_ptr<generic::Appender> g_appender = std::make_shared<generic::ConsoleAppender>();
//std::shared_ptr<generic::LoggerBase> g_logger = std::make_shared<generic::AsyncLogger>(g_appender);

int main()
{
    LOG_SET_LOGLEVEL(generic::LogLevel::TRACE);
    LOG_SHOW_TIMESTAMP(true);
    // LOG_SET_LOGGER(g_logger);

    generic::Config config({"", 2048, false, 1});
    generic::TCPServer server(config);
    server.setOnMessage(
        [](std::shared_ptr<generic::TCPConnection> conn) {
            char temp[64];
            int n = snprintf(temp, 64, "msg from %s: ", conn->getPeerAddr().toString().c_str());
            generic::TCPConnection::BufferPtr buf = conn->getReadBuffer();
            LOG_INFO << generic::SimpleString(temp, n) << generic::SimpleString(buf->read_ptr(), buf->data_size()) << " size: " << buf->data_size();
        });
    server.init();
    server.start();
    return 0;
}