#define _GNU_SOURCE

#include "func.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>

#define CLASS_NO 1
#define GROUP_NO 6

/*
 * Função para calcular a diferença entre dois instantes temporais timespec
 */
struct timespec timeDiff(struct timespec end, struct timespec start)
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

/*
 * Função para converter timespec em ms 
 */
long double timeToMs(struct timespec a)
{
    return (long double)(a.tv_sec * (long double)1E3 + a.tv_nsec / (long double)1E6);
}

int main(int argc, char **argv)
{
    struct timespec start, end; //Variaveis para guardar o tempo
    long double times[3];       //Array para guardar os tempos de computação
    cpu_set_t mask;             //cpu_set_t represents a set of CPU's

    /* Lock all of the calling process's virtual address space into RAM */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
        perror("main->mlockall");
    }

    /* Faz com que o programa corra em apenas um core do CPU, neste caso o core 0 */
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);

    /* Set a thread's cpu affinity mask, main thread */
    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) == -1)
    {
        perror("main->sched_setaffinity");
    }

    for (int i = 0; i < 3; i++)
    {
        /* Gets starting time */
        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
        {
            perror("error clock");
        }
        /* Starts the task */
        if (i == 0)
        {
            f1(CLASS_NO, GROUP_NO);
        }
        if (i == 1)
        {
            f2(CLASS_NO, GROUP_NO);
        }
        if (i == 2)
        {
            f3(CLASS_NO, GROUP_NO);
        }
        /* Gets the time when the task ended */
        if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
        {
            perror("error clock");
        }

        /* Calculates the difference */
        times[i] = timeToMs(timeDiff(end, start));
        printf("f%d: %LF ms\n", i, times[i]);
    }
}
