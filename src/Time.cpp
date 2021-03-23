#include "Time.h"

#include <sys/time.h>
#include <ctime>
#include <cstdio>

using namespace generic;

void Time::update()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    m_sec = time.tv_sec;
    m_usec = time.tv_usec;
    m_tm = gmtime(&m_sec);
}


// static
int Time::toFormatString(Time &time, char *buf, int maxlen)
{
    //std::thread::id tid = std::this_thread::get_id();
    int len = snprintf(buf, maxlen, "%4d%02d%02d %02d:%02d:%02d.%06d",
                       time.m_tm->tm_year + 1900, time.m_tm->tm_mon + 1, time.m_tm->tm_mday,
                       time.m_tm->tm_hour, time.m_tm->tm_min, time.m_tm->tm_sec, time.m_usec);
    return len;
}

//int toFormatString(Time &time, Buffer* buf){}