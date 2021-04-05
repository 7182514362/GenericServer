#ifndef UTIL_H
#define UTIL_H

#include <unistd.h>
#include <stdio.h>

namespace generic
{
    template <typename T>
    int convert(char *buf, T value);

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

}

#endif //UTIL_H