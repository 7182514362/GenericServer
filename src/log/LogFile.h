#ifndef LOG_FILE_H
#define LOG_FILE_H

#include "Noncopyable.h"
#include "FileUtil.h"
#include "Time.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <memory>

namespace generic
{
#define LOGFILE_SIZE 1 << 26
    class LogFile : Noncopyable
    {
    private:
        std::unique_ptr<FileUtil> m_file;
        Time m_time;

    public:
        LogFile() : m_file(new FileUtil(genFileName(), LOGFILE_SIZE)), m_time()
        {
            m_file->open();
        }
        LogFile(std::string name) : m_file(new FileUtil(name, LOGFILE_SIZE)), m_time()
        {
            m_file->open();
        }
        ~LogFile()
        {
            m_file->close();
        }

        std::string genFileName();

        void append(const char *data, int len);

    private:
        bool exist(std::string &filename) const
        {
            return m_file->exist();
        }
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