#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <unistd.h>
#include <cstdio>

namespace generic
{
    template <typename T>
    int convert(char *buf, T value);

#define ASSERT(expr) assert(expr)

    class Util
    {
    public:
        template <typename T>
        static int convert(char *buf, T value);

        static int write(int fd, const void *buf, int count)
        {
            ssize_t n = ::write(fd, buf, count);
            if (n < 0)
            {
                perror("Util::write");
            }
            return n;
        }

        static int read(int fd, void *buf, int count)
        {
            ssize_t n = ::read(fd, buf, count);
            if (n < 0)
            {
                perror("Util::read");
            }
            return n;
        }
    };

    class StringWrapper //: Noncopyable
    {
    public:
        StringWrapper(const char *str, int len) : m_str(str), m_len(len) {}

        const char *m_str;
        const int m_len;
    };

}

#endif //UTIL_H