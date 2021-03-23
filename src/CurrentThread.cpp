#include "CurrentThread.h"
#include <unistd.h>
#include <sys/syscall.h>

using namespace generic;

thread_local int CurrentThread::s_tid = 0;

int CurrentThread::tid()
{
    if (s_tid == 0)
    {
        s_tid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
    return s_tid;
}