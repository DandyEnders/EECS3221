#include "stdlib.h"
#include "stdio.h"

#include "schedulers.h"

#define INITIAL_HEAD_POSITION 33

/*
*/

// works like an order added
int unique_id = 0;


// add a task to the list 
void add(int cylinder_request){
    printf("adding %d\n", cylinder_request);
}

void print_result(  
                    FILE* output, 
                    char* algorithm_type, 
                    char* seek_sequence_out, 
                    int head_movement
                )
{
    fprintf(output, "\n%s Algorithm\n", algorithm_type);
    fprintf(output, "Initial head psoition = %d\n", INITIAL_HEAD_POSITION);
    fprintf(output, "Seek Sequence is:\n");
    fprintf(output, "%s", seek_sequence_out);
    fprintf(output, "Total head movement for %s = %d\n", algorithm_type, head_movement);
    // for debug

    printf("\n%s Algorithm\n", algorithm_type);
    printf("Initial head psoition = %d\n", INITIAL_HEAD_POSITION);
    printf("Seek Sequence is:\n");
    printf("%s", seek_sequence_out);
    printf("Total head movement for %s = %d\n", algorithm_type, head_movement);
}

void fcfs_schedule(FILE* output){
    int head_movement = 0;
    char* seek_seq_str = "\n";

    print_result(output, "FCFS", seek_seq_str, head_movement);
}

void scan_schedule(FILE* output){
    int head_movement = 0;
    char* seek_seq_str = "\n";

    print_result(output, "SCAN", seek_seq_str, head_movement);
}

void cscan_schedule(FILE* output){
    int head_movement = 0;
    char* seek_seq_str = "\n";

    print_result(output, "C_SCAN", seek_seq_str, head_movement);
}

// invoke the scheduler
void schedule(FILE* output){
    printf("scheduling start\n");
    fcfs_schedule(output);
    scan_schedule(output);
    cscan_schedule(output);
    
}


// #define RUNTIME_SLICE current_task->burst;
// #define QUANTUM_SLICE current_task->burst <= QUANTUM ? current_task->burst: QUANTUM;

// #define IMPLEMENTATION_SLICE RUNTIME_SLICE

// // Arrival queue
// Queue *task_arriaval_queue;
// Queue *task_queue;
// int last_tid = 1;

// // add a task to the list 
// void add(char *name, int priority, int burst){
//     if(task_arriaval_queue == NULL){
//         task_arriaval_queue = create_empty_queue();
//     }

//     Task *newTask = malloc(sizeof(Task));
    
//     newTask->tid = last_tid; 
//     last_tid = last_tid + 1;
    
//     newTask->name = name;
//     newTask->priority = priority;
//     newTask->burst = burst;

//     enqueue(task_arriaval_queue, newTask);
// }

// // selects next task to run, dispatch process to CPU.
// Task* pickNextTask(){
//     return dequeue(task_queue);
// }

// // invoke the scheduler
// void schedule(){
//     task_queue = copy_queue(task_arriaval_queue);

//     int task_count = task_arriaval_queue->size;

//     int n_remaining_task = task_count;
//     int current_time_unit = 0;

//     int total_waiting_time = 0;
//     int total_turnraround_time = 0;
//     int total_response_time = 0;

//     //int *turnaround_time = calloc(task_count, sizeof(int));
//     int *response_time = calloc(task_count, sizeof(int));

//     float avg_waiting_time = 0;
//     float avg_turnaround_time = 0;
//     float avg_response_time = 0;

//     int i = 0;
//     int time_quantum;
//     Task *current_task;
//     while((current_task = pickNextTask()) != NULL){
//         time_quantum = IMPLEMENTATION_SLICE;

//         run(current_task, time_quantum);

//         int current_task_index = current_task->tid - 1;
        
//         // when task is for the first time responded,
//         if(response_time[current_task_index] == 0){
//             response_time[current_task_index] = 1;
//             total_response_time += current_time_unit;
//         }

//         current_time_unit += time_quantum;
//         current_task->burst -= time_quantum;

//         // while T1 is running for 20 units, T2..T8 waits 20 units.
//         // T1's_total_waiting_time = T1_burst_time * len(T2..T8)
//         total_waiting_time += time_quantum * (n_remaining_task - 1);

//         // when task is done running,
//         if(current_task->burst == 0){
//             n_remaining_task -= 1;
//             //turnaround_time[current_task_index] = current_time_unit;
//             total_turnraround_time += current_time_unit;
//         }  

//         // variant
//         i += 1;
//     } 

//     avg_waiting_time = (float) total_waiting_time / (float) task_count;
//     avg_turnaround_time = (float) total_turnraround_time / (float) task_count;
//     avg_response_time = (float) total_response_time / (float) task_count;

//     printf("Average Turnaround time: %f, Waiting time:%f, Response time:%f\n",
//             avg_turnaround_time, avg_waiting_time, avg_response_time);
// }
