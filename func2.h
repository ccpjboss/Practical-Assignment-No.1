#ifndef FUNC2_H
#define FUNC2_H

/* Tempos de execução das tasks (aproximados) */
#define T1 30000000
#define T2 50000000
#define T3 80000000

/* Tasks */
void f1(int,int);
void f2(int,int);
void f3(int,int);

/* Set a standard format time to timespec format */
struct timespec timespecFormat(int seconds, int nanoseconds);

#endif