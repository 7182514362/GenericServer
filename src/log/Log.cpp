#include "Log.h"
#include "Util.h"
#include "CurrentThread.h"

#include <functional>
#include <string>
#include <fcntl.h>
#include <unistd.h>

#include <mutex>

using namespace generic;

/**************** LoggerBase start ****************/

/**************** LoggerBase   end ****************/

/**************** SyncLogger start ****************/

SyncLogger::SyncLogger() : SyncLogger(std::make_shared<ConsoleAppender>())
{
}

SyncLogger::SyncLogger(std::shared_ptr<Appender> appender)
    : m_buffer(std::make_shared<SyncLoggerBuffer>()), m_appender(appender)
{
}

void SyncLogger::append(const char *data, int len)
{
    m_buffer->append(data, len);
};

void SyncLogger::SyncLogger::output()
{
    m_appender->append(m_buffer->read_ptr(), m_buffer->data_size());
    m_buffer->clear();
}

/**************** SyncLogger   end ****************/

/**************** AsyncLogger start ****************/

#define ASYNCLOGGER_BUFFER_SIZE (1 << 15)

AsyncLogger::AsyncLogger(std::shared_ptr<Appender> appender)
    : m_appender(appender),
      m_bufferMap(std::make_shared<std::map<const int, RingBufferPtr>>()),
      m_thread(),
      m_running(false)
{
    assert(m_appender);
    start();
}

AsyncLogger::~AsyncLogger()
{
    stop();
}
void AsyncLogger::start()
{
    assert(m_running == false);
    m_running = true;
    assert(m_thread == nullptr);
    m_thread.reset(new std::thread(std::bind(&AsyncLogger::threadFunc, this)));
}

void AsyncLogger::stop()
{
    m_running = false;
    //m_cond.notify_one();
    m_thread->join();
}

void AsyncLogger::append(const char *data, int size)
{
    int tid = CurrentThread::tid();
    if (m_bufferMap->find(tid) == m_bufferMap->end())
    {
        static std::mutex mtx;
        std::lock_guard<std::mutex> lock(mtx);
        (*m_bufferMap)[tid] = std::make_shared<RingBuffer>(ASYNCLOGGER_BUFFER_SIZE);
    }

    RingBufferPtr buffer = (*m_bufferMap)[tid];

    int n = buffer->append(data, size);
    int remain = size - n;
    while (remain > 0)
    {
        n = buffer->append(data, size);
        remain -= n;
    }
}

// run in thread
void AsyncLogger::threadFunc()
{
    int buffer_size = 1 << 20;
    char *buffer = new char[buffer_size];
    int highWaterMark = buffer_size - 1024;
    memset(buffer, 0, buffer_size);

    int n = 0;
    while (m_running)
    {
        for (auto &buf : *m_bufferMap)
        {
            while (!buf.second->empty())
            {
                n += buf.second->retrive(buffer + n, buffer_size - n);
                if (n > highWaterMark)
                {
                    m_appender->append(buffer, n);
                    n = 0;
                }
            }
        }
        m_appender->append(buffer, n);
        n = 0;
    }
    // flush
    for (auto &buf : *m_bufferMap)
    {
        while (!buf.second->empty())
        {
            n += buf.second->retrive(buffer + n, buffer_size - n);
            if (n > highWaterMark)
            {
                m_appender->append(buffer, n);
                n = 0;
            }
        }
    }
    m_appender->append(buffer, n);
    delete[] buffer;
}

/**************** AsyncLogger end ****************/

/**************** Logger start ****************/

std::map<LogLevel, std::string> Logger::s_logLevelName = {
    {LogLevel::TRACE, "TRACE "},
    {LogLevel::DEBUG, "DEBUG "},
    {LogLevel::INFO, " INFO "},
    {LogLevel::WARN, " WARN "},
    {LogLevel::ERROR, "ERROR "},
    {LogLevel::FATAL, "FATAL "}};

// static
LogLevel Logger::logLevel = LogLevel::INFO;

//static thread_local(since cpp11)
thread_local std::shared_ptr<LoggerBase> Logger::s_logger = nullptr;
bool Logger::s_showTimeStamp(false);

Logger::Logger(const char *source, int line, LogLevel level) : m_source(source), m_line(line)
{
    if (!s_logger)
    {
        // default logger
        s_logger.reset(new SyncLogger());
    }

    format(level);
}

Logger::~Logger()
{
    m_stream << " -" << m_source << ":" << m_line << "\n";
    s_logger->append(m_stream.buffer()->read_ptr(), m_stream.buffer()->data_size());
    s_logger->output();
}

void Logger::format(LogLevel level)
{
    char buf[64];
    int n = 0;
    if (s_showTimeStamp)
    {
        Time time;
        n = time.toFormatString(time, buf, 64);
        buf[n++] = ' ';
    }
    m_stream << StringWrapper(buf, n) << CurrentThread::tid() << " " << s_logLevelName[level];
}

/**************** Logger   end ****************/
