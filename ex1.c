#include "func.h"
#include <stdio.h>
#include <errno.h>
#include <time.h>

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

/*
 *Função para converter em ms 
 */
long double timespecInMs(struct timespec a) //return ms
{
    return (long double)(a.tv_sec * (long double)1E3 + a.tv_nsec / (long double)1E6);
}

int main(int argc, char **argv)
{
    struct timespec start, end; //Variaveis para guardar o tempo
    long double times[3];       //Array para guardar os tempos de computação

    for (int i = 0; i < 3; i++)
    {
        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
        {
            perror("error clock");
        }
        if (i == 0)
        {
            f1(0, 0);
        }
        if (i == 1)
        {
            f2(0, 0);
        }
        if (i == 2)
        {
            f3(0, 0);
        }
        if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
        {
            perror("error clock");
        }

        times[i] = timeToMs(timeDiff(end, start));
        printf("f%d: %LF ms\n", i, times[i]);
    }
}