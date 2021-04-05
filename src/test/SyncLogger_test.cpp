#include <iostream>
#include <memory>
#include <unistd.h>

#include "Log.h"

using namespace std;

int main()
{
    int num = 1000;
    for (int i = 0; i < num; ++i)
    {
        LOG_INFO << "logging test:" << i;
    }

    return 0;
}