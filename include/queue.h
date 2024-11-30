#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct QueueItem {
    struct QueueItem *next;
    void *data;
} QueueItem;

typedef struct
{
    QueueItem *first;
    QueueItem *last;
} Queue;

void Queue_Init(Queue *queue);

/// @brief Queue can only be destroyed if it's empty.
/// @param queue 
/// @return 0 if queue was destroyed, -1 otherwise.
int Queue_Destroy(Queue *queue);

bool Queue_IsEmpty(Queue *queue);

int Queue_Enqueue(Queue *queue, void *data);
void Queue_Dequeue(Queue *queue, void **out);

#endif // QUEUE_H
