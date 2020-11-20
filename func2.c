#include "func2.h"
#include <time.h>
#include <stdio.h>

struct timespec timespecFormat(int seconds, int nanoseconds)
{
    struct timespec result;

    result.tv_sec = (time_t)(seconds + (int)(nanoseconds / (int)1E9));
    result.tv_nsec = (long)(nanoseconds % (int)1E9);

    return result;
}

void f1(int a, int b)
{
    struct timespec time;
    time = timespecFormat((int)0, (int)T1);

    if (nanosleep(&time, NULL) != 0)
    {
        perror("nanosleep");
    }
}

void f2(int a, int b)
{
    struct timespec time;
    time = timespecFormat((int)0, (int)T2);

    if (nanosleep(&time, NULL) != 0)
    {
        perror("nanosleep");
    }
}

void f3(int a, int b)
{
    struct timespec time;
    time = timespecFormat((int)0, (int)T3);

    if (nanosleep(&time, NULL) != 0)
    {
        perror("nanosleep");
    }
}