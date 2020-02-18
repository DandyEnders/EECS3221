#include "string.h"
#include "queue.h"
#include "stdio.h"
#include "stdlib.h"

struct node* create_new_node(Task *a_task){
    struct node *newNode = malloc(sizeof(struct node));
    newNode->task = a_task;
    newNode->next = NULL;
    return newNode;
}

// queue
Queue* create_empty_queue(){
    Queue* temp_queue = malloc(sizeof(Queue));
    temp_queue->head = NULL;
    temp_queue->tail = NULL;
    temp_queue->cursor = NULL;
    temp_queue->size = 0;
    return temp_queue;
}

void enqueue(Queue *a_queue, Task *a_task){
    struct node* a_node = create_new_node(a_task);
    if(is_empty(a_queue)){
        a_queue->head = a_node;
        a_queue->tail = a_node;
    }else{
        a_queue->tail->next = a_node;
        a_queue->tail = a_node;
    }
    a_queue->size = a_queue->size + 1;
}

Task* dequeue(Queue *a_queue){
    if(is_empty(a_queue)){
        return NULL;
    }else if(a_queue->size == 1){
        struct node* temp;
        temp = a_queue->head;
        a_queue->head = NULL;
        a_queue->tail = NULL;
        a_queue->cursor = NULL;
        a_queue->size = 0;

        return temp->task;
    }else{ // size more than 1
        struct node* temp;
        temp = a_queue->head;
        a_queue->head = a_queue->head->next;
        a_queue->size = a_queue->size - 1;

        return temp->task;
    }
}

int is_empty(Queue *a_queue){
    return a_queue->size == 0;
}

Task* first(Queue *a_queue){
    if (is_empty(a_queue)){
        return NULL;
    }else{
        return a_queue->head->task;
    }
}

Task* last(Queue *a_queue){
    if (is_empty(a_queue)){
        return NULL;
    }else{
        return a_queue->tail->task;
    }
}

// list operator
void list_delete(Queue *a_queue, Task *a_task){
    if(is_empty(a_queue)){
        return;
    }else{
        struct node *temp;
        struct node *prev;

        temp = a_queue->head;
        // special case - beginning of list
        if (strcmp(a_task->name, temp->task->name) == 0) {
            if(a_queue->size == 1){
                a_queue->head = NULL;
                a_queue->tail = NULL;
                a_queue->cursor = NULL;
                a_queue->size = 0;
            }else{
                a_queue->head = a_queue->head->next;
                a_queue->size -= 1;
            }
        }
        else {
            // interior or last element in the list
            prev = a_queue->head;
            temp = prev->next;

            while (strcmp(a_task->name, temp->task->name) != 0) {
                prev = temp;
                temp = temp->next;
            }

            // if temp is tail,
            if(strcmp(temp->task->name, a_queue->tail->task->name) == 0){
                a_queue->tail = prev;
            }
            prev->next = temp->next;
            a_queue->size -= 1;
        }
    }
}


// Iterator
void start(Queue *a_queue){
    a_queue->cursor = a_queue->head;
}

Task* next(Queue *a_queue){
    if(a_queue->cursor == NULL){
        return NULL;
    }else{
        Task* temp_task = a_queue->cursor->task;
        a_queue->cursor = a_queue->cursor->next;
        return temp_task;
    }
}

Task* item(Queue *a_queue){
    if(a_queue->cursor == NULL){
        return NULL;
    }else{
        return a_queue->cursor->task;
    }
}

Task* cursor_remove(Queue *a_queue){
    if(a_queue->cursor == NULL){
        return NULL;
    }else if(a_queue->size == 1){
        struct node* temp;
        temp = a_queue->cursor;
        a_queue->head = NULL;
        a_queue->tail = NULL;
        a_queue->cursor = NULL;
        a_queue->size = 0;

        return temp->task;
    }else{ // size more than 1
        struct node* temp;
        temp = a_queue->cursor;
        a_queue->cursor = a_queue->cursor->next;
        a_queue->size = a_queue->size - 1;

        return temp->task;
    }
}


// copy
Queue* copy_queue(Queue *a_queue){
    Queue* new_queue;
    new_queue = create_empty_queue();
    struct node *temp;
    temp = a_queue->head;

    while (temp != NULL) {
        Task *old_task = temp->task;
        Task *new_task = malloc(sizeof(Task));
        
        new_task->name = malloc(sizeof(char*) * strlen(old_task->name));

        strcpy(new_task->name, old_task->name);
        new_task->tid = old_task->tid;
        new_task->priority = old_task->priority;
        new_task->burst = old_task->burst;

        enqueue(new_queue, new_task);
        temp = temp->next;
    }
    return new_queue;
}