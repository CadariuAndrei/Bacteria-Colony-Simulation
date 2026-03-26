/* File:
 *    hello1.c
 *  It illustrates a method to pass a rank value to created threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Global variable:  accessible to all threads */
int thread_count;

/* Thread function. Receives one argument which is a pointer to its rank.
    All threads will get this function as their thread function.
 */
void *Hello(void *rank)
{
   int my_rank = *(int *)rank;

   printf("Hello from thread %d of %d\n", my_rank, thread_count);

   return NULL;
}

void *Ciao(void *rank)
{
   int my_rank = *(int *)rank;

   printf("Ciao from thread %d of %d\n", my_rank, thread_count);

   return NULL;
}
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
   int i; 

   pthread_t *thread_handles;
   
   int *tid;

   /* initializes threadcount by hardcoding value. Could read it*/
   thread_count = 10; 

   thread_handles = malloc(thread_count * sizeof(pthread_t));

   tid = malloc(thread_count * sizeof(int));


   /* Each thread gets the same thread function Hello to execute.
      Each thread gets the current value of the counter i
      as argument to its thread function. This acts as its rank */
   for (i = 0; i < thread_count; i++)
   {
	tid[i] = i;

   if(tid[i]%2!=0)
   {
	pthread_create(&thread_handles[i], NULL,
                     Hello, (void *)&tid[i]);
   }
   
   if(tid[i]%2==0)
   {
      pthread_create(&thread_handles[i], NULL,
                     Ciao, (void *)&tid[i]);
   }
   }

   printf("Hello from the main thread\n");

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);

   free(thread_handles);
   free(tid);
   return 0;
}