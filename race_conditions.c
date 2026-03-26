#include<stdio.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 2

int count=0;
pthread_mutex_t count_mutex;

void *inc_count(void *t)
{
    int my_id=*(int *)t;
    pthread_mutex_lock(&count_mutex);
    count++;
    pthread_mutex_unlock(&count_mutex);
    return NULL;
}
int main(int argc,char **argv)
{

    pthread_t threads[NUM_THREADS];
    int tid[NUM_THREADS];

    pthread_mutex_init(&count_mutex,NULL);
    for(int i=0;i<NUM_THREADS;i++)
    {
        tid[i]=i;
        pthread_create(&threads[i],NULL,inc_count,(void*)&tid[i]);
    }

    for(int i=0;i<NUM_THREADS;i++)
    {
        pthread_join(threads[i],NULL);
    }

    printf("The final value of count=%d\n",count);

    pthread_mutex_destroy(&count_mutex);
    return 0;
}