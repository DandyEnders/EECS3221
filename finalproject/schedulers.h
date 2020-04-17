// add a task to the list 
void add(int cylinder_request);

// selects next task to run, dispatch process to CPU.
int pickNextRequest();

// invoke the scheduler
void schedule();

