/**
 * Driver.c
 * schedule <input.txt> <initial_head_pos> output.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    FILE *out;
    char *temp;
    char task[SIZE];

    int request;
    
    in = fopen(argv[1],"r");
    int initial_head_position = atoi(argv[2]);
    out = fopen(argv[3],"w");

    init();
    
    while (fgets(task,SIZE,in) != NULL) {
        temp = strtok(task, "\n");
        request = atoi(strsep(&temp,"\n"));

        // add the task to the scheduler's list of tasks
        add(request);

        free(temp);
    }

    
    fclose(in);
    
    // invoke the scheduler
    schedule(out, initial_head_position);

    fclose(out);
    

    return 0;
}
