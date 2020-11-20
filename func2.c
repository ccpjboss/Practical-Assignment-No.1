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

bool timeMenor(struct timespec a, struct timespec b)
{
    if (a.tv_sec < b.tv_sec)
    {
        return true;
    }
    else
    {
        if (a.tv_sec == b.tv_sec)
        {
            if (a.tv_nsec < b.tv_nsec)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}
/*
 * Função para calcular a diferença entre dois instantes temporais
 */
struct timespec timeDiff(struct timespec end, struct timespec start) // return ms
{
    struct timespec result;
    if ((end.tv_nsec - start.tv_nsec) < 0)
    {
        result.tv_sec = end.tv_sec - start.tv_sec - 1;
        result.tv_nsec = 1E9 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        result.tv_sec = end.tv_sec - start.tv_sec;
        result.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return result;
}

/* Para simular as tasks de func.h usamos a system call nanosleep que "adormece" a thread por x ns */
void f1(int a, int b)
{
    wait(T1);
}

void f2(int a, int b)
{
    wait(T2);
}

void f3(int a, int b)
{
    wait(T3);
}

void wait(int time)
{
    struct timespec cur, start;
    struct timespec t = timespecFormat((int)0, (int)time);

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    {
        perror("clock_gettime_func2");
    }

    cur = start;
    while (timeMenor(timeDiff(cur, start), t))
    {
        if (clock_gettime(CLOCK_MONOTONIC, &cur) == -1)
        {
            perror("clock_gettime_func2");
        }
    }
}