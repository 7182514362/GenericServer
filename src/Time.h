#ifndef TIME_H
#define TIME_H

#include <ctime>
#include "Util.h"

namespace generic
{
    class Time
    {
    public:
        Time()
        {
            update();
        }

        void update();

        static int toFormatString(Time &time, char *buf, int maxlen);
        //static int toFormatString(Time &time, Buffer* buf);

    public:
        long m_sec;
        long m_usec;
        struct tm *m_tm;
    };
}

#endif