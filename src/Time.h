#ifndef TIME_H
#define TIME_H

#include "Util.h"
#include <time.h>

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

        static long distance_from_now_sec(const Time& t);

        static int format_string(Time &time, char *buf, int maxlen);
        //static int toFormatString(Time &time, Buffer* buf);

    public:
        long m_sec;
        long m_usec;
        struct tm *m_tm;
    };
}

#endif