#include "Time.h"

#include <sys/time.h>
#include <ctime>
#include <cstdio>
#include <cmath>

using namespace generic;

void Time::update()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    m_sec = time.tv_sec;
    m_usec = time.tv_usec;
    m_tm = gmtime(&m_sec);
}

long Time::distance_from_now_sec(const Time &t)
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return std::abs(t.m_sec - time.tv_sec);
}

// static
int Time::format_string(Time &time, char *buf, int maxlen)
{
    //std::thread::id tid = std::this_thread::get_id();
    int len = snprintf(buf, maxlen, "%4d%02d%02d %02d:%02d:%02d.%06d",
                       time.m_tm->tm_year + 1900, time.m_tm->tm_mon + 1, time.m_tm->tm_mday,
                       time.m_tm->tm_hour, time.m_tm->tm_min, time.m_tm->tm_sec, time.m_usec);
    return len;
}

//int toFormatString(Time &time, Buffer* buf){}