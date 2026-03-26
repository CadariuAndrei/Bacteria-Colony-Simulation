/* File:  abc.c
Creates 3 threads.
First thread repeatedly writes A.
Second thread repeatredly writes B.
Third thread repeatedly writes C.
The main thread repeatedly writes newline.

Add synchronization such that the output is always in the form:
on each new line there is a random permutation of A,B,C.
You can add any synchronization that you want, but you 
can not change the existing printf's or add new printf's

For example, the output can be:
ABC
BAC
ABC
BCA
...
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REPEAT 100

pthread_barrier_t my_barrier1;
pthread_barrier_t my_barrier2;

/* Thread function A */
void *HelloA(void *dummy)
{
    for (int i = 0; i < REPEAT; i++)
    {
       printf("A");
       pthread_barrier_wait(&my_barrier1);
       pthread_barrier_wait(&my_barrier2);
    }

    return NULL;
}

/* Thread function B */
void *HelloB(void *dummy)
{
    for (int i = 0; i < REPEAT; i++)
    {
        
       printf("B");
       pthread_barrier_wait(&my_barrier1);
       pthread_barrier_wait(&my_barrier2);
    }

    return NULL;
}

/* Thread function C */
void *HelloC(void *dummy)
{
    for (int i = 0; i < REPEAT; i++)
    {
        printf("C");
        pthread_barrier_wait(&my_barrier1);
        pthread_barrier_wait(&my_barrier2);
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    pthread_t thread_handleA, thread_handleB, thread_handleC;

    pthread_barrier_init(&my_barrier1,NULL,4);
    pthread_barrier_init(&my_barrier2,NULL,4);
 
    pthread_create(&thread_handleA, NULL, HelloA, NULL);

    pthread_create(&thread_handleB, NULL, HelloB, NULL);

    pthread_create(&thread_handleC, NULL, HelloC, NULL);

    for (int i = 0; i < REPEAT; i++)
    {   
        pthread_barrier_wait(&my_barrier1);
        printf("\n");
        pthread_barrier_wait(&my_barrier2);
    }


    pthread_join(thread_handleA, NULL);

    pthread_join(thread_handleB, NULL);

    pthread_join(thread_handleC, NULL);

    pthread_barrier_destroy(&my_barrier1);
    pthread_barrier_destroy(&my_barrier2);

    return 0;
}