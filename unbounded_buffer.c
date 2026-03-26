#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REPEAT 5    /* each thread is doing REPEAT operations  */
#define NUM_THREADS 4

/* mutex lock protecting access to the buffer */
pthread_mutex_t lock;

/* condition variable to signal when buffer
is not empty - wakes up a waiting consumer  */
pthread_cond_t not_empty_cv;

typedef struct node
{
    int data;
    struct node *next;
} queueNode;

queueNode *head = NULL;
queueNode *tail = NULL;

int isEmpty(void)
{
    return (head == NULL);
}

void init_synchro()
{
    /* Initialize mutex */
    pthread_mutex_init(&lock, NULL);
     /* Initialize cond vars */
    pthread_cond_init(&not_empty_cv, NULL);
}

void destroy_synchro()
{
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_empty_cv);
}

void put(int val)
{
    struct node *new = malloc(sizeof(struct node));
    new->data = val;
    new->next = NULL;

    if (!isEmpty())
    {
        tail->next = new;
        tail = new;
    }
    else
    {
        head = new;
        tail = new;
    }
}

int get()
{
    if (isEmpty())
    {
        printf("Error! empty queue \n");
        exit(1);
    }
    struct node *oldhead = head;
    int rez = oldhead->data;
    head = head->next;
    if (head == NULL)
        tail = NULL;
    free(oldhead);
    return rez;
}

/* 
thread function for producer threads.
thread 0 produces: 0 1 2 3 4
thread 2 produces: 20 21 22 23 24 
*/
void *producer(void *t)
{
    int i;
    int my_id = *(int *)t;

    for (i = 0; i < REPEAT; i++)
    {
        pthread_mutex_lock(&lock);

        put(i + my_id * 10);

        pthread_cond_signal(&not_empty_cv);
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

/* thread function for consumer threads */
void *consumer(void *t)
{
    int i;
    int my_id = *(int *)t;

    for (i = 0; i < REPEAT; i++)
    {
        pthread_mutex_lock(&lock);

        while(isEmpty())
        {
            pthread_cond_wait(&not_empty_cv,&lock);
        }
        int rez =get();
        pthread_mutex_unlock(&lock);
        printf("Consumer thread %d got %d \n", my_id, rez);
    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    /* Init cond var and mutex */
    init_synchro();

    /* Create threads */
    /* odd thread ranks are consumers, even ranks are producers */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        ids[i] = i;
        if (i % 2 == 0)
            pthread_create(&threads[i], NULL, producer, (void *)&ids[i]);
        else
            pthread_create(&threads[i], NULL, consumer, (void *)&ids[i]);
    }

    /* Wait for all threads to complete */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    /* Clean up */
    destroy_synchro();
    return 0;
}