#include <iostream>
#include <memory>
#include <thread>
#include <functional>
#include <unistd.h>

#include "Log.h"
#include "Appender.h"

using namespace std;

//std::make_shared<generic::FileAppender>("test.log");
//std::make_shared<generic::FileAppender>("test.log");
std::shared_ptr<generic::Appender> g_appender = std::make_shared<generic::ConsoleAppender>();
std::shared_ptr<generic::LoggerBase> g_logger = std::make_shared<generic::AsyncLogger>(g_appender);

void bench_1()
{
    LOG_SET_LOGGER(g_logger);
    int num = 1000;
    for (int i = 0; i < num; ++i)
    {
        LOG_INFO << "log test bench_1: " << i;
        usleep(1000);
    }
}

void bench_2()
{
    LOG_SET_LOGGER(g_logger);
    int num = 1000;
    for (int i = 0; i < num; ++i)
    {
        LOG_WARN << "log test bench_2: " << i;
        usleep(1000);
    }
}

void bench_3()
{
    LOG_SET_LOGGER(g_logger);
    int num = 1000;
    for (int i = 0; i < num; ++i)
    {
        LOG_DEBUG << "log test bench_3: " << i;
        usleep(1000);
    }
}

int main()
{
    LOG_SET_LOGGER(g_logger);
    LOG_SHOW_TIMESTAMP(true);
    LOG_SET_LOGLEVEL(generic::LogLevel::DEBUG);

    std::thread thread_1(bench_1);
    std::thread thread_2(bench_2);
    std::thread thread_3(bench_3);

    thread_1.join();
    thread_2.join();
    thread_3.join();

    return 0;
}