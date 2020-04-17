/**
 * list data structure containing the tasks in the system
 */

#include "task.h"

#ifndef NODE_H
#define NODE_H

struct node {
    Task *task;
    struct node *next;
} Node;

#endif

// insert and delete operations.
void insert(struct node **head, Task *task);
void delete(struct node **head, Task *task);
void traverse(struct node *head);
