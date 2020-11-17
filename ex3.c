#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>

#include "func.h"

#define GROUP_NO 1 
#define CLASS_NO 1

/**
 * Set a standard format time to timespec format
 * */
struct timespec timespecFormat(int seconds, int nanoseconds)
{
    struct timespec result;

    result.tv_sec = (time_t)(seconds + (int)(nanoseconds / (int)1E9));
    result.tv_nsec = (long)(nanoseconds % (int)1E9);

    return result;
}

/**
 * Sums two timespec stuctures
 **/
struct timespec timeSum(struct timespec a, struct timespec b)
{
    struct timespec result;

    result.tv_nsec = a.tv_nsec + b.tv_nsec;
    if (result.tv_nsec > (int)1E9 - 1)
    {
        result.tv_nsec = result.tv_nsec % (int)1E9;
        result.tv_sec = 1 + a.tv_sec + b.tv_sec;
    }
    else
    {
        result.tv_sec = a.tv_sec + b.tv_sec;
    }
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
 * Função para converter timespec em ms 
 */
long double timeToMs(struct timespec a) //return ms
{
    return (long double)(a.tv_sec * (long double)1E3 + a.tv_nsec / (long double)1E6);
}

//Struct with the input of the thread function call
struct threadInput
{
    int function;
    struct timespec period;
    struct timespec start;
    struct timespec end;
};

//Struct with the output of the thread
struct threadOut
{
    int expected;
    int done;
    int *deadline;
};

void *performWorK(void *input)
{
    struct timespec next, cur;
    struct threadInput *in = (struct threadInput *)input;
    struct threadOut *output = (struct threadOut *)malloc(1 * sizeof(struct threadOut));

    if (output == NULL)
    {
        perror("thread malloc");
    }

    /**
     * Calculate how many times it is expected to run and allocates a table to store
     * the computation time of the task
     */
    output->expected = (int)((float)6 / ((float)timeToMs(in->period) / (float)1E3) + 0.5);
    output->deadline = (int *)malloc(output->expected * sizeof(int));

    if (output->deadline == NULL)
    {
        perror("malloc_thread");
    }

    next = in->start;

    int i = 0;

    while (timeMenor(next, in->end))
    {
        if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL) != 0)
        {
            printf("1\n");
            perror("nanosleep");
            pthread_exit(NULL);
        }

        if (in->function == 0)
        {
            f1(CLASS_NO, GROUP_NO);
        }
        if (in->function == 1)
        {
            f2(CLASS_NO, GROUP_NO);
        }
        if (in->function == 2)
        {
            f3(CLASS_NO, GROUP_NO);
        }

        if (clock_gettime(CLOCK_MONOTONIC, &cur) == -1)
        {
            perror("clock_gettime");
        }

        next = timeSum(next, in->period);
        if (i < output->expected)
        {
            if (timeMenor(cur, next))
            {
                output->deadline[i] = 1;
            }
            else
            {
                output->deadline[i] = 0;
            }
        }
        i++;
    }
    output->done = i;
    pthread_exit((void *)output);
}

int main()
{
    cpu_set_t mask;
    struct timespec start, finish;
    pthread_attr_t attr[3];
    struct sched_param sched[3];
    struct threadInput input[3];
    struct threadOut *output[3];
    pthread_t thread[3];

    int periodos[3] = {100000000, 200000000, 400000000};

    //Prevents the memory from being paged to the swap area
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1)
    {
        perror("mklockall");
    }

    CPU_ZERO(&mask);   //Clears cpu set, so that it contains no CPU's
    CPU_SET(0, &mask); //Add a cpu to the set, the 0 represents the cpu 0

    //Set a thread's cpu affinity mask, use 0 in the first argument for calling thread
    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask))
    {
        perror("sched_affinity");
    }

    //Setting the start time and finish time
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) //Gets the universal start time
    {
        perror("clock_gettime(start)");
    }

    start = timeSum(start, timespecFormat((int)2, 0)); //Adds 3 seconds to the start time to make sure that all the thread are created
    finish = timeSum(start, timespecFormat((int)6, 0));

    for (int i = 0; i < 3; i++)
    {
        //Initiates the atributes of the thread
        if (pthread_attr_init(&(attr[i])) != 0)
        {
            perror("pthread_attr_init");
        }

        //Sets the thread affinity to the cpu_set_t making it run in core 0, in this case
        if (pthread_attr_setaffinity_np(&(attr[i]), sizeof(cpu_set_t), &mask) != 0)
        {
            perror("pthread_setaffinity_np");
        }

        /**
         * Takes their scheduling attribut from the values specified by the attr object 
         */
        if (pthread_attr_setinheritsched(&(attr[i]), PTHREAD_EXPLICIT_SCHED) != 0)
        {
            perror("pthread_setinheritshed");
        }

        /**
         * Sets the scheduling policy attribute of the thread attributes refered by attr
         * to the value SCHED_FIFO
         */
        if (pthread_attr_setschedpolicy(&(attr[i]), SCHED_FIFO) != 0)
        {
            perror("phread_attr_setschedpolicy");
        }

        //Gets the maximum priority for the policy in use
        memset(&(sched[i]), 0, sizeof(struct sched_param));
        if ((sched[i].sched_priority = sched_get_priority_max(SCHED_FIFO)) == -1)
        {
            perror("main->sched_get_priority_max");
        }

        //Decrements the priority, thread 1 will get max priority
        sched[i].sched_priority -= i;
        printf("Priority of thread %d: %d\n", i + 1, sched[i].sched_priority);

        //Sets the priority of the thread
        if (pthread_attr_setschedparam(&(attr[i]), &(sched[i])) != 0)
        {
            perror("pthread_attr_setschedparam");
        }

        input[i].function = i;
        input[i].period = timespecFormat(0, periodos[i]);
        input[i].start = start;
        input[i].end = finish;
    }

    for (int i = 0; i < 3; i++)
    {
        if (pthread_create(&(thread[i]), &(attr[i]), performWorK, &(input[i])) != 0)
        {
            perror("pthread_create");
        }
    }

    /* Waits for the threads to finish and destoys the attr */
    for (int i = 0; i < 3; i++)
    {
        if (pthread_join(thread[i], (void *)&(output[i])) != 0)
        {
            perror("thread join");
        }

        if (pthread_attr_destroy(&(attr[i])) != 0)
        {
            perror("attr_destroy");
        }
    }

    /* Presents the computation times */
    for (int i = 0; i<3; i++)
    {
        printf("f%d:\n",i+1);
        if (output[i] != NULL)
        {
            printf("Execuções esperadas %d  Execuções: %d\n", output[i]->expected, output[i]->done);
            for (int j = 0;j<output[i]->done || j<output[i]->expected;j++)
            {
                printf("N. %d: Periodo = %d ms ",j,periodos[i]/(int)1E6);
                if (output[i]->deadline[j])
                {
                    printf("fulfilled\n");
                }
                else
                {
                    printf("not fullfilled\n");
                }
            }
            free(output[i]->deadline);
            free(output[i]);
        }
    }
    exit(EXIT_SUCCESS);
}