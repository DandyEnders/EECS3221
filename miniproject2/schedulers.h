#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

#include "task.h"
#include "queue.h"

Queue *task_arriaval_queue;
Queue *task_queue;

// add a task to the list 
void add(char *name, int priority, int burst);

// selects next task to run, dispatch process to CPU.
Task *pickNextTask();

// invoke the scheduler
void schedule();

