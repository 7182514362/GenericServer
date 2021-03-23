#include "EchoServer.h"
#include "Log.h"
#include "Config.h"

using namespace std;

//std::shared_ptr<generic::Appender> g_appender = std::make_shared<generic::ConsoleAppender>();
//std::shared_ptr<generic::LoggerBase> g_logger = std::make_shared<generic::AsyncLogger>(g_appender);

int main()
{
    LOG_SET_LOGLEVEL(generic::LogLevel::TRACE);
    LOG_SHOW_TIMESTAMP(true);
    Config config = {"0.0.0.0", 2048, false, 1};
    EchoServer server(config);
    server.start();
    return 0;
}