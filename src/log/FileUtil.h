#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include "Noncopyable.h"
#include <string>

namespace generic
{
#define DEFAULT_FILE_SIZE 1 << 25
    class FileUtil : Noncopyable
    {
        int m_fd;
        const int MAX_SIZE;

    public:
        std::string m_name;
        long m_writtenBytes;

    public:
        FileUtil(std::string name, int maxsize);
        ~FileUtil();
        void open();

        void close();
        // may exceed MAX_SIZE
        int write(const char *data, int len);
        int read(char *buf, long maxlen) const;
        bool exist() const;

        bool reachMaxSize()
        {
            return m_writtenBytes >= MAX_SIZE;
        }

    private:
        inline void open_internal(int flags);
        //void write_internal(const char *data, int len);
    };

    class Console : Noncopyable
    {
    private:
    public:
        int write(const char *buf, int len);

        int read(char *buf, int bytes);
    };
}
#endif