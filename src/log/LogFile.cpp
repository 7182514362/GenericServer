#include "LogFile.h"
#include <assert.h>
#include <string.h>

using namespace generic;

LogFile::LogFile() : m_file(new FileUtil(genFileName(), LOGFILE_SIZE)), m_time()
{
    m_file->open();
}
LogFile::LogFile(std::string name) : m_file(new FileUtil(name, LOGFILE_SIZE)), m_time()
{
    m_file->open();
}
LogFile::~LogFile()
{
    m_file->close();
}

std::string LogFile::genFileName()
{
    std::string name;
    m_time.update();
    char buf[64];
    int n = strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S.", m_time.m_tm); //%Y%m%d-%H%M%S
    assert(n > 0);
    name.append(buf, n);
    name += std::to_string(::getpid());
    name += ".log";
    return name;
}

void LogFile::append(const char *data, int len)
{
    if (len > LOGFILE_SIZE)
    {
        printf("LogFile::append(): data too large!\n");
        return;
    }

    int ret = m_file->write(data, len);
    if (ret == -1)
    {
        m_file->close();
        std::string name = genFileName();
        m_file.reset(new FileUtil(name, LOGFILE_SIZE));
        m_file->open();
        m_file->write(data, len);
    }
}

bool LogFile::exist(std::string &filename) const
{
    return m_file->exist();
}

void LogConsole::append(const char *data, int len)
{
    m_console.write(data, len);
}