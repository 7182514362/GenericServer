#include "Util.h"
#include <algorithm>

static const char digits[] = "9876543210123456789";
static const char *zero = digits + 9;
template <typename T>
int convert(char *buf, T value)
{
    T i = value;
    char *p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}