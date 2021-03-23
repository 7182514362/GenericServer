#ifndef LAYOUT_H
#define LAYOUT_H

#include "Time.h"
#include "CurrentThread.h"

#include <cstring>
#include <string>
//#include <memory>

namespace generic
{
    /*  %D : Data
        %T : time
        %t : thread id
        %L : log message
    */

    class Layout
    {
        std::string m_format;

        //Time m_time;

    public:
        Layout(std::string format) : m_format(format)
        {
        }

        void format(char *buf, int buf_size, const char *logmsg, int msglen)
        {
            const char *c = m_format.c_str();
            const char *end = buf + buf_size - 1;

            // true when *c=='%
            bool begin = false;
            Time time;
            while (*c != '\0' && buf <= end)
            {
                switch (*c)
                {
                case '%':
                    ++c;
                    begin = true;

                    break;
                case 'D':
                    int len1 = snprintf(buf, 9, "%4d%02d%02d",
                                        time.m_tm->tm_year + 1900, time.m_tm->tm_mon + 1, time.m_tm->tm_mday);
                    buf += len1;
                    ++c;
                    begin = false;
                    break;
                case 'T':
                    int len2 = snprintf(buf, 13, "%02d:%02d:%02d.%06d",
                                        time.m_tm->tm_hour, time.m_tm->tm_min, time.m_tm->tm_sec, time.m_usec);
                    buf += len2;
                    ++c;
                    begin = false;
                case 't':
                    int len3 = convert(buf, CurrentThread::tid());
                    buf += len3;
                    ++c;
                    begin = false;
                    break;
                case 'L':
                    strncpy(buf, logmsg, msglen);
                    buf += msglen;
                    ++c;
                    begin = false;
                    break;
                default:
                    *buf = *c;
                    ++buf;
                    ++c;
                    begin = false;
                    break;
                }
            }
            *buf = '\0';
        }

        void formatTime(Time &time, const char *format, char *buf, int maxlen)
        {
            int len = snprintf(buf, maxlen, "%4d%02d%02d-%02d:%02d:%02d:%06d",
                               time.m_tm->tm_year + 1900, time.m_tm->tm_mon + 1, time.m_tm->tm_mday,
                               time.m_tm->tm_hour, time.m_tm->tm_min, time.m_tm->tm_sec, time.m_usec);
        }

    public:
        static const char *s_logLevelName[6];
    };

}

#endif