/* File:
 *    mutex.c
 * It illustrates the use of Mutexes for threads synchronization
 * Main creates a number of NUM_THREADS=2 threads
 * There is a shared variable count, initialized with 0
 * The two threads both increment count and use a mutex for mutual exclusion
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 2

/* shared variable */
int count = 0;

/* mutex to protect critical regions
when threads are updating count */
pthread_mutex_t count_mutex;

/* thread function */
void *inc_count(void *t)
{
  int i;
  int my_id = *(int *)t;

  pthread_mutex_lock(&count_mutex);
  count++;
  printf("Thread %d incremented count to %d \n", my_id, count);
  pthread_mutex_unlock(&count_mutex);
  return NULL;
}

int main(int argc, char *argv[])
{

  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  /* Initialize mutex */
  pthread_mutex_init(&count_mutex, NULL);

  /* Create incrementer threads */
  for (int i = 0; i < NUM_THREADS; i++)
  {
    ids[i] = i;
    pthread_create(&threads[i], NULL, inc_count, (void *)&ids[i]);
  }

  /* Wait for all threads to complete */
  for (int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(threads[i], NULL);
  }

  printf("Final value of count = %d \n", count);

  /* Clean up */
  pthread_mutex_destroy(&count_mutex);
}