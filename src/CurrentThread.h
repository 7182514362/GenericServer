#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

#include "Noncopyable.h"

namespace generic
{

    class CurrentThread : Noncopyable
    {
    private:
        static thread_local int s_tid;

    public:
        static int tid();
    };
}

#endif