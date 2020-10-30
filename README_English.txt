Example of the command line to compile a program in C in the file "program.c" (to be developed by you) that links to the object file "func.o" (having the object code of the functions f1(), f2() and f3()), creating the executable file "program":

gcc -Wall -O2 -D_REENTRANT -lpthread -lrt program.c func.o -o program
or
gcc -Wall -O2 -D_REENTRANT program.c func.o -lpthread -lrt -o program

The program.c file must include the func.h header (#include "func.h").
