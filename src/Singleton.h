#ifndef SINGLETON_H
#define SINGLETON_H
#include "Noncopyable.h"

namespace generic
{
    template <typename T>
    class Singleton : Noncopyable
    {
    private:
        Singleton();

    public:
        static T *getInstance()
        {
            static T instance;
            return &instance;
        }
    };
} // namespace generic
#endif //SINGLETON_H