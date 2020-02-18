#include "stdlib.h"

#include "task.h"
#include "list.h"
#include "cpu.h"

struct node **head;
int last_tid = 0;

// add a task to the list 
void add(char *name, int priority, int burst){
    Task *newTask = malloc(sizeof(struct task));

    newTask->tid = last_tid; 
    last_tid = last_tid + 1;
    
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;

    insert(head, newTask);
}

// selects next task to run, dispatch process to CPU.
Task *pickNextTask(){
    Task *head_task;
    struct node *temp;
    temp = *head;
    head_task = temp->task;
    delete(head, head_task);
    return head_task;
}

// invoke the scheduler
void schedule(){
    Task *next_task;
    while(*head != NULL){
        next_task = pickNextTask();
        run(next_task, next_task->burst);
    }

}

