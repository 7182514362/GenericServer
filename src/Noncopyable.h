#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace generic
{
    class Noncopyable
    {
    protected:
        Noncopyable() = default;
        ~Noncopyable() = default;

        Noncopyable(const Noncopyable &) = delete;
        Noncopyable &operator=(const Noncopyable &) = delete;
    };
}

#endif //NONCOPYABLE_H