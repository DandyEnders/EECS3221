#include "list.h"
#include "task.h"

#ifndef SLL_QUEUE_H
#define SLL_QUEUE_H

typedef struct singly_linked_list {
    struct node *head;
    struct node *tail;
    struct node *cursor; // for iteration
    int size;
} Queue;

#endif

Queue*  create_empty_queue();

void    enqueue(Queue *a_queue, Task *a_task);
Task*   dequeue(Queue *a_queue);
int     is_empty(Queue *a_queue);
Task*   first(Queue *a_queue);
Task*   last(Queue *a_queue);

// list operator
void    list_delete(Queue *a_queue, Task *a_task);

void    start(Queue *a_queue);
Task*   next(Queue *a_queue);
Task*   item(Queue *a_queue);
Task*   cursor_remove(Queue *a_queue);

Queue*  copy_queue(Queue *a_queue);