#include <stdlib.h>

#include "structs/queue.h"

void Queue_Init(Queue *queue)
{
    queue->first = NULL;
    queue->last = NULL;
}

int Queue_Destroy(Queue *queue)
{
    if (queue->first != NULL)
    {
        return -1;
    }
    return 0;
}

bool Queue_IsEmpty(Queue *queue) {
    return queue->first == NULL;
}

int Queue_Enqueue(Queue *queue, void *data)
{
    // Allocate memory for the new item
    QueueItem *item = malloc(sizeof(QueueItem));
    if (item == NULL)
    {
        return -1;
    }
    item->data = data;
    item->next = NULL;

    if (queue->first == NULL)
    {
        // Queue is empty. Set first and last.
        queue->first = item;
        queue->last = item;
        return 0;
    }

    // Otherwise, append to last
    queue->last->next = item;
    queue->last = item;
    return 0;
}

void Queue_Dequeue(Queue *queue, void **out)
{
    // If queue is empty, return NULL
    if (queue->first == NULL)
    {
        *out = NULL;
    }

    // Remove first item from the queue
    QueueItem *item = queue->first;
    queue->first = item->next;

    // If the queue is now empty, set last to NULL
    if (queue->first == NULL)
    {
        queue->last = NULL;
    }

    // Return the data and free the QueueItem
    out = item->data;
    free(item);
}
