#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// add a task to the list 
void add(char *name, int priority, int burst);

// selects next task to run, dispatch process to CPU.
void pickNextTask();

// invoke the scheduler
void schedule();

