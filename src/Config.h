#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace generic
{

    struct Config
    {
        std::string ip;
        int port;
        bool use_ipv6;
        int work_thread_num;
    };
}

#endif