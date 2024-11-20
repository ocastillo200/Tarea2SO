#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "include/circularQ.h"
#include "include/vmSimulator.h"

int main(int argc, char *argv[])
{
    int num_producers = 2, num_consumers = 2, queue_size = 8, max_wait_time = 5;

    CircularQueue q;
    initQueue(&q, queue_size);

    pthread_t producers[num_producers], consumers[num_consumers];
    for (int i = 0; i < num_producers; i++)
    {
        pthread_create(&producers[i], NULL, producer, &q);
    }
    for (int i = 0; i < num_consumers; i++)
    {
        pthread_create(&consumers[i], NULL, consumer, &q);
    }

    for (int i = 0; i < num_producers; i++)
    {
        pthread_join(producers[i], NULL);
    }
    sleep(max_wait_time);
    for (int i = 0; i < num_consumers; i++)
    {
        pthread_join(consumers[i], NULL);
    }

    free(q.buffer);
    return 0;
}
