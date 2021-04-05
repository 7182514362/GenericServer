#ifndef LOG_FILE_H
#define LOG_FILE_H

#include "Noncopyable.h"
#include "FileUtil.h"
#include "Time.h"

#include <string>
#include <memory>

namespace generic
{
#define LOGFILE_SIZE 1 << 26
    class LogFile : Noncopyable
    {

    public:
        LogFile();
        LogFile(std::string name);
        ~LogFile();

        std::string genFileName();

        void append(const char *data, int len);

        bool exist(std::string &filename) const;

    private:
        std::unique_ptr<FileUtil> m_file;
        Time m_time;
    };

    class LogConsole : Noncopyable
    {
    private:
        Console m_console;

    public:
        LogConsole() : m_console() {}
        void append(const char *data, int len);
    };
}

#endif