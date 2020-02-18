#include "stdlib.h"
#include "stdio.h"

#include "queue.h"
#include "schedulers.h"
#include "cpu.h"

Queue *task_queue;
int last_tid = 1;

// add a task to the list 
void add(char *name, int priority, int burst){
    if(task_queue == NULL){
        task_queue = malloc(sizeof(Queue));
    }
    Task *newTask = malloc(sizeof(Task));
    
    newTask->tid = last_tid; 
    last_tid = last_tid + 1;
    
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    enqueue(task_queue, newTask);
}

// selects next task to run, dispatch process to CPU.
Task *pickNextTask(){
    return dequeue(task_queue);
}

// invoke the scheduler
void schedule(){
    Task *next_task;
    while((next_task = pickNextTask()) != NULL){
        run(next_task, next_task->burst);
    }

}

