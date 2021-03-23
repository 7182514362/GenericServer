#ifndef APPENDER_H
#define APPENDER_H

#include "Noncopyable.h"
#include "LogFile.h"
#include "Buffer.h"

#include <memory>

namespace generic
{
    class Appender : Noncopyable
    {
    public:
        virtual void append(const char *data, int len) = 0;
    };

    class FileAppender : public Appender
    {
    private:
        LogFile m_logFile;

    public:
        FileAppender() : m_logFile() {}
        FileAppender(std::string filename) : m_logFile(filename) {}
        void append(const char *data, int len) override
        {
            m_logFile.append(data, len);
        }
    };

    class ConsoleAppender : public Appender
    {
    private:
        LogConsole m_logConsole;

    public:
        ConsoleAppender() {}
        void append(const char *data, int len) override
        {
            if (data == nullptr)
            {
                m_logConsole.append("invalid data\n", 13);
                abort();
            }
            m_logConsole.append(data, len);
        }
    };

    // no implement
    /*
    class NetAppender : public IAppender
    {
    public:
        void append(const char *data, int len) override {}
    };
    */

}

#endif