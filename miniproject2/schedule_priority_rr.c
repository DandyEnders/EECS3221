#include "stdlib.h"
#include "stdio.h"

#include "queue.h"
#include "schedulers.h"
#include "cpu.h"

#define RUNTIME_SLICE current_task->burst;
#define QUANTUM_SLICE current_task->burst <= QUANTUM ? current_task->burst: QUANTUM;

#define IMPLEMENTATION_SLICE QUANTUM_SLICE

// Arrival queue
Queue *task_arriaval_queue;
Queue *task_queue;
int last_tid = 1;

int time_quantum = 0;

// add a task to the list 
void add(char *name, int priority, int burst){
    if(task_arriaval_queue == NULL){
        task_arriaval_queue = create_empty_queue();
    }

    Task *newTask = malloc(sizeof(Task));
    
    newTask->tid = last_tid; 
    last_tid = last_tid + 1;
    
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;

    enqueue(task_arriaval_queue, newTask);
}

// selects next task to run, dispatch process to CPU.
Task* pickNextTask(){
    if(is_empty(task_queue)){
        return NULL;
    }else{
        Task* highest_priority_task = task_queue->head->task;
        
        start(task_queue);
        Task* cursor_task;
        while((cursor_task = next(task_queue)) != NULL){
            if(cursor_task->priority > highest_priority_task->priority){
                highest_priority_task = cursor_task;
            }
        }
        list_delete(task_queue, highest_priority_task);

        if(task_queue->head != NULL){
            Task* next_highest_priority_task = task_queue->head->task;
            
            start(task_queue);
            while((cursor_task = next(task_queue)) != NULL){
                if(cursor_task->priority == highest_priority_task->priority){
                    next_highest_priority_task = cursor_task;
                }
            }
            
            if(next_highest_priority_task != NULL){
                if(next_highest_priority_task->priority != highest_priority_task->priority){
                    time_quantum = highest_priority_task->burst;
                }else{
                    int run_time = highest_priority_task->burst <= QUANTUM ? highest_priority_task->burst: QUANTUM;;
                    if(highest_priority_task->burst - run_time > 0){
                        enqueue(task_queue, highest_priority_task);
                    }
                    time_quantum = run_time;
                }
            }else{
                time_quantum = highest_priority_task->burst;
            }
        }
        
        
        

        return highest_priority_task;
    }
}
// invoke the scheduler
void schedule(){
    task_queue = copy_queue(task_arriaval_queue);

    int task_count = task_arriaval_queue->size;

    int n_remaining_task = task_count;
    int current_time_unit = 0;

    int total_waiting_time = 0;
    int total_turnraround_time = 0;
    int total_response_time = 0;

    //int *turnaround_time = calloc(task_count, sizeof(int));
    int *response_time = calloc(task_count, sizeof(int));

    float avg_waiting_time = 0;
    float avg_turnaround_time = 0;
    float avg_response_time = 0;

    int i = 0;
    
    Task *current_task;
    while((current_task = pickNextTask()) != NULL){
        //time_quantum = IMPLEMENTATION_SLICE;
        
        run(current_task, time_quantum);

        int current_task_index = current_task->tid - 1;
        
        // when task is for the first time responded,
        if(response_time[current_task_index] == 0){
            response_time[current_task_index] = 1;
            total_response_time += current_time_unit;
        }

        current_time_unit += time_quantum;
        current_task->burst -= time_quantum;

        // while T1 is running for 20 units, T2..T8 waits 20 units.
        // T1's_total_waiting_time = T1_burst_time * len(T2..T8)
        total_waiting_time += time_quantum * (n_remaining_task - 1);

        // when task is done running,
        if(current_task->burst == 0){
            n_remaining_task -= 1;
            //turnaround_time[current_task_index] = current_time_unit;
            total_turnraround_time += current_time_unit;
        }  

        // variant
        i += 1;
    } 

    avg_waiting_time = (float) total_waiting_time / (float) task_count;
    avg_turnaround_time = (float) total_turnraround_time / (float) task_count;
    avg_response_time = (float) total_response_time / (float) task_count;

    printf("Average Turnaround time: %f, Waiting time:%f, Response time:%f\n",
            avg_turnaround_time, avg_waiting_time, avg_response_time);
}
