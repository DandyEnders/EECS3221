#include "stdlib.h"
#include "stdio.h"
#include <string.h>

#include "schedulers.h"

// #define INITIAL_HEAD_POSITION 33
#define N_MAX_CYLINDERS 100

#define NO_REQUEST -1

/* 
    Global variables
*/

// Cylinder reqeust representation will be filled with 
// id of requests. 
// For SCAN and C_SCAN
// -1 means never requested
int cylinder[N_MAX_CYLINDERS];

// store requests in the order of incoming.
// for FCFS
// -1 means no requests
int requests[N_MAX_CYLINDERS];
// keep track of where the next available request is
int available_requests_idx = 0; 

// id = an order added
int unique_id = 0;

int initial_head_position = 33;

void init(){
    for(int i = 0; i< N_MAX_CYLINDERS; i++){
        cylinder[i] = NO_REQUEST;
    }

    for(int i = 0; i< N_MAX_CYLINDERS; i++){
        requests[i] = NO_REQUEST;
    }
}

int fetch_id(){
    int old_unique_id = unique_id;
    unique_id = unique_id + 1;
    return old_unique_id;
}

// add a task to the list 
void add(int cylinder_request){
    cylinder[cylinder_request] = fetch_id();

    requests[available_requests_idx] = cylinder_request;
    available_requests_idx = available_requests_idx + 1;
}

void print_result(  
                    FILE* output, 
                    char* algorithm_type, 
                    char* seek_sequence_out, 
                    int head_movement
                ){
    fprintf(output, "\n%s Algorithm\n", algorithm_type);
    fprintf(output, "Initial head position = %d\n", initial_head_position);
    fprintf(output, "Seek Sequence is:\n");
    fprintf(output, "%s", seek_sequence_out);
    fprintf(output, "Total head movement for %s = %d\n", algorithm_type, head_movement);
}

void fcfs_schedule(FILE* output){
    int head_movement = 0;
    char seek_seq_str[(N_MAX_CYLINDERS * 11)] = "";
    int current_head = initial_head_position;
    int distance;

    for(int i = 0; i < available_requests_idx; i ++){
        distance = abs(requests[i] - current_head);
        current_head = requests[i];
        head_movement = head_movement + distance;

        char temp[10] = "";
        sprintf(temp, "%d\n", requests[i]);
        strcat(seek_seq_str, temp);
    }

    print_result(output, "FCFS", seek_seq_str, head_movement);
}

void scan_schedule(FILE* output){
    int head_movement = 0;
    char seek_seq_str[(N_MAX_CYLINDERS * 11)] = "";
    int last_valid_head = initial_head_position;
    int distance;

    int cylinder_copy[N_MAX_CYLINDERS];

    for(int i = 0; i < N_MAX_CYLINDERS; i++){
        cylinder_copy[i] = cylinder[i];
    }

    // go all valid left
    for(int i = last_valid_head; i > 0; i--){
        if(cylinder_copy[i] != NO_REQUEST){
            distance = abs(i - last_valid_head);
            last_valid_head = i;
            head_movement = head_movement + distance;

            char temp[10] = "";
            sprintf(temp, "%d\n", i);
            strcat(seek_seq_str, temp);
            // request handled
            cylinder_copy[i] = NO_REQUEST;
        }
    }

    int is_more_request_on_right = 0;

    for(int i = initial_head_position; i < N_MAX_CYLINDERS; i++){
        if(cylinder_copy[i] != NO_REQUEST){
            is_more_request_on_right = 1;
        }
    }

    if(is_more_request_on_right == 1){
        // Go all the way left
        distance = abs(0 - last_valid_head);
        last_valid_head = 0;
        head_movement = head_movement + distance;

        // go all valid right
        for(int i = 0; i < N_MAX_CYLINDERS; i++){
            if(cylinder_copy[i] != NO_REQUEST){
                distance = abs(i - last_valid_head);
                last_valid_head = i;
                head_movement = head_movement + distance;

                char temp[10] = "";
                sprintf(temp, "%d\n", i);
                strcat(seek_seq_str, temp);
                // request handled
                cylinder_copy[i] = NO_REQUEST;
            }
        }
    }

    

    print_result(output, "SCAN", seek_seq_str, head_movement);
}

void cscan_schedule(FILE* output){
    int head_movement = 0;
    char seek_seq_str[(N_MAX_CYLINDERS * 11)] = "";
    int last_valid_head = initial_head_position;
    int distance;

    int cylinder_copy[N_MAX_CYLINDERS];

    for(int i = 0; i < N_MAX_CYLINDERS; i++){
        cylinder_copy[i] = cylinder[i];
    }

    // go all valid right
    for(int i = last_valid_head; i < N_MAX_CYLINDERS; i++){
        if(cylinder_copy[i] != NO_REQUEST){
            distance = abs(i - last_valid_head);
            last_valid_head = i;
            head_movement = head_movement + distance;

            char temp[10] = "";
            sprintf(temp, "%d\n", i);
            strcat(seek_seq_str, temp);
            // request handled
            cylinder_copy[i] = NO_REQUEST;
        }
    }

    int is_more_request_on_left = 0;

    for(int i = initial_head_position; i > 0; i--){
        if(cylinder_copy[i] != NO_REQUEST){
            is_more_request_on_left = 1;
        }
    }

    if(is_more_request_on_left == 1){
        // Go all the way right
        distance = abs((N_MAX_CYLINDERS - 1) - last_valid_head);
        last_valid_head = (N_MAX_CYLINDERS - 1);
        head_movement = head_movement + distance;

        // Go all the way left
        distance = abs(0 - last_valid_head);
        last_valid_head = 0;
        head_movement = head_movement + distance;

        // go all valid right
        for(int i = 0; i < N_MAX_CYLINDERS; i++){
            if(cylinder_copy[i] != NO_REQUEST){
                distance = abs(i - last_valid_head);
                last_valid_head = i;
                head_movement = head_movement + distance;

                char temp[10] = "";
                sprintf(temp, "%d\n", i);
                strcat(seek_seq_str, temp);
                // request handled
                cylinder_copy[i] = NO_REQUEST;
            }
        }
    }

    

    print_result(output, "C_SCAN", seek_seq_str, head_movement);
}

// invoke the scheduler
void schedule(FILE* output, int init_head_pos){
    // printf("scheduling start\n");
    initial_head_position = init_head_pos;
    fcfs_schedule(output);
    scan_schedule(output);
    cscan_schedule(output);
    
}
