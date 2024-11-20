#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <pthread.h>

typedef struct
{
    int *buffer;
    int size;
    int count;
    int front;
    int rear;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} CircularQueue;

void initQueue(CircularQueue *q, int size);
void resizeQueue(CircularQueue *q, int new_size);
void enqueue(CircularQueue *q, int item);
int dequeue(CircularQueue *q);

#endif