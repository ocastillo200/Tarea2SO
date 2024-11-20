#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "include/circularQ.h"

void initQueue(CircularQueue *q, int size)
{
    q->buffer = (int *)malloc(size * sizeof(int));
    q->size = size;
    q->count = 0;
    q->front = 0;
    q->rear = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void resizeQueue(CircularQueue *q, int new_size)
{
    int *new_buffer = (int *)malloc(new_size * sizeof(int));
    for (int i = 0; i < q->count; i++)
    {
        new_buffer[i] = q->buffer[(q->front + i) % q->size];
    }
    free(q->buffer);
    q->buffer = new_buffer;
    q->size = new_size;
    q->front = 0;
    q->rear = q->count;
}

void enqueue(CircularQueue *q, int item)
{
    pthread_mutex_lock(&q->lock);
    while (q->count == q->size)
    {
        resizeQueue(q, q->size * 2);
    }
    q->buffer[q->rear] = item;
    q->rear = (q->rear + 1) % q->size;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

int dequeue(CircularQueue *q)
{
    pthread_mutex_lock(&q->lock);
    while (q->count == 0)
    {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    int item = q->buffer[q->front];
    q->front = (q->front + 1) % q->size;
    q->count--;
    if (q->count > 0 && q->count <= q->size / 4)
    {
        resizeQueue(q, q->size / 2);
    }
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return item;
}

void *producer(void *arg)
{
    CircularQueue *q = (CircularQueue *)arg;
    for (int i = 0; i < 100; i++)
    {
        enqueue(q, i);
        usleep(100000);
    }
    return NULL;
}

void *consumer(void *arg)
{
    CircularQueue *q = (CircularQueue *)arg;
    for (int i = 0; i < 100; i++)
    {
        int item = dequeue(q);
        printf("Consumido: %d\n", item);
        usleep(150000);
    }
    return NULL;
}
