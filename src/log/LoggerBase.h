#ifndef LOGGERBASE_H
#define LOGGERBASE_H
#include "Noncopyable.h"

namespace generic
{
    enum class LogLevel
    {
        TRACE = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    class Time;

    class LoggerBase : Noncopyable
    {
    public:
        LoggerBase() = default;
        virtual ~LoggerBase() = default;
        virtual void append(const char *data, int len) = 0;
        virtual void output() = 0;
        //virtual void format(LogLevel level, Time &time) = 0;
    };
}

#endif