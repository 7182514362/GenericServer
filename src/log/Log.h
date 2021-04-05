#ifndef LOG_H
#define LOG_H

#include "Noncopyable.h"
#include "LogStream.h"
#include "LoggerBase.h"

#include <atomic>
#include <thread>
#include <memory>
#include <map>

namespace generic
{
    class Appender;
    class Buffer;
    template <int N>
    class FixedBuffer;
    class RingBuffer;
    class SimpleString;

#define SYNCLOGGER_BUFFER_SIZE (1 << 10)
    class SyncLogger : public LoggerBase
    {
        typedef FixedBuffer<SYNCLOGGER_BUFFER_SIZE> SyncLoggerBuffer;
        typedef std::shared_ptr<SyncLoggerBuffer> SyncLoggerBufferPtr;

        SyncLoggerBufferPtr m_buffer;
        std::shared_ptr<Appender> m_appender;

    public:
        SyncLogger();
        SyncLogger(std::shared_ptr<Appender> appender);
        ~SyncLogger() = default;

        void append(const char *data, int len) override;
        void output() override;
    };

    class AsyncLogger : public LoggerBase
    {
        using RingBufferPtr = std::shared_ptr<RingBuffer>;

    private:
        std::shared_ptr<Appender> m_appender;
        //LogCircularBuffer m_circularBuffer;
        std::shared_ptr<std::map<const int, RingBufferPtr>> m_bufferMap;
        std::unique_ptr<std::thread> m_thread;

        std::atomic<bool> m_running;

    public:
        AsyncLogger(std::shared_ptr<Appender> appender);
        ~AsyncLogger();

        void start();
        void stop();
        // need time control
        void append(const char *data, int size) override;
        // do nothing, threadFunc will do output
        virtual void output() override {}
        // run in one thread
        void threadFunc();
    };

    class Logger
    {
    private:
        static std::map<LogLevel, SimpleString> s_logLevelName;
        static bool s_showTimeStamp;
        // is static necessary?
        static thread_local std::shared_ptr<LoggerBase> s_logger;

        const char *m_source;
        int m_line;

        LogStream m_stream;

    public:
        static LogLevel logLevel;

    public:
        Logger(const char *source, int line, LogLevel level);
        ~Logger();

        static void setLoglevel(LogLevel level) { logLevel = level; }

        static void setLogger(std::shared_ptr<LoggerBase> &logger)
        {
            s_logger = logger;
        }

        static void setShowTimeStamp(bool b)
        {
            s_showTimeStamp = b;
        }

        LogStream &stream() { return m_stream; }

        void format(LogLevel level);
    };

} // namespace generic

// __FILENAME__ defined in cmake files
#define LOG_TRACE                                              \
    if (generic::Logger::logLevel <= generic::LogLevel::TRACE) \
    generic::Logger(__FILENAME__, __LINE__, generic::LogLevel::TRACE).stream()
#define LOG_DEBUG                                              \
    if (generic::Logger::logLevel <= generic::LogLevel::DEBUG) \
    generic::Logger(__FILENAME__, __LINE__, generic::LogLevel::DEBUG).stream()
#define LOG_INFO                                              \
    if (generic::Logger::logLevel <= generic::LogLevel::INFO) \
    generic::Logger(__FILENAME__, __LINE__, generic::LogLevel::INFO).stream()
#define LOG_WARN                                              \
    if (generic::Logger::logLevel <= generic::LogLevel::WARN) \
    generic::Logger(__FILENAME__, __LINE__, generic::LogLevel::WARN).stream()
#define LOG_ERROR                                              \
    if (generic::Logger::logLevel <= generic::LogLevel::ERROR) \
    generic::Logger(__FILENAME__, __LINE__, generic::LogLevel::ERROR).stream()
#define LOG_FATAL                                              \
    if (generic::Logger::logLevel <= generic::LogLevel::FATAL) \
    generic::Logger(__FILENAME__, __LINE__, generic::LogLevel::FATAL).stream()

#define LOG_SET_LOGGER(logger) generic::Logger::setLogger(logger)
#define LOG_SET_LOGLEVEL(level) generic::Logger::setLoglevel(level)
#define LOG_SHOW_TIMESTAMP(boolean) generic::Logger::setShowTimeStamp(boolean)

#endif /* LOG_H */