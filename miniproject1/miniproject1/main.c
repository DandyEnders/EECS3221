#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_LINE 80 /* The maximum length command */
#define HALF_LINE MAX_LINE/2 + 1

/*

    Author : Jinho Hwang
    Project description : https://github.com/hamzehkhazaei/EECS3221_MiniProj1/blob/master/Project_Description_and_Grading.md

    Objective:
    1. Creating the child process and executing the command in the child
    2. Providing a history feature
    3. Adding support of input and output redirection
    4. Allowing the parent and child processes to communicate via a pipe

*/

//void my_prune_string(char* string){
//    int i;
//    int j;
//    int cur_len = strlen(string);
//    if(cur_len > 0){
//        for(i = 0; i < cur_len; i++){
//            if(string[i] < '!'){
//                for(j = i; j < cur_len - 1; j++){
//                    string[j] = string[j+1];
//                }
//                string[cur_len - 1] = '\0';
//            }
//        }
//    }
//
//}

int my_strcmp(char* string1, char* string2){
    if(strcmp(string1, string2) == 0){
        return 1;
    }else{
        return 0;
    }
}

int main(void) {
//    char *args[HALF_LINE]; /* command line arguments */
    char args[MAX_LINE] = "\0";
    char prev_args[MAX_LINE] = "\0"; // previous args;
    char *args_token[HALF_LINE];
    char *commands[3];
    char *storage;

    char *last_arg;

    int should_run = 1; /* flag to determine when to exit program */

    fflush(stdout);

    while (should_run){
        printf("mysh:~$ ");
        fflush(stdout);

        fgets(args, MAX_LINE, stdin);
        if(args[strlen(args)-1] == '\n'){
            args[strlen(args)-1] = '\0';
        }
//        printf("input: %s", args);

        // if args is something weird
        char dummy_arg[MAX_LINE] = "\0";
        strcpy(dummy_arg, args);
        if(strtok(dummy_arg, " \n") == NULL){
//            printf("NULL: %s", args);
            continue;
        }

//        printf("%s", args);
        if(my_strcmp(args, "!!")){
            if(my_strcmp(prev_args, "\0")){
                printf("No commands in history.\n");
                continue;
            }else{
//              args = prev_args;
                strcpy(args, prev_args);
            }
        }else{
//          success mode so put prev_args now
//          prev_args = args;
            strcpy(prev_args, args);
        }
        // Tokenize args
        int i = 0;
        storage = strtok(args, " \n");
        while(storage != NULL){
            args_token[i] = storage;
//            printf("storage: %s", storage);
            storage = strtok(NULL, " \n");
            i = i + 1;
        }


        // if last token is &
        int is_background_process = my_strcmp(args_token[i-1], "&");
        if(is_background_process){
            args_token[i-1] = NULL;
        }

        // for now just use first three
        commands[0] = args_token[0];
        commands[1] = args_token[1];
        commands[2] = args_token[2];

        // exit then just exit
        if(my_strcmp(commands[0], "exit")){
            should_run = 0;

        // if not then execution mode
        }else{
            int my_pipe[2];

            // Forking
            pid_t pid;
    //        int status;
            pid = fork();
            //if pid < 0, error occured.
            if(pid < 0){
                printf("You failed fork: %s\n", stderr);
            // if pid == 0, child process.
            }else if(pid == 0){
//                printf("Child process: %s, %s, %s\n", commands[0], commands[1], commands[2]);

                execvp(commands[0], commands);
            // if pid > 0, parent process.
            }else{
                // if last arg is &, it == 0.
                if(is_background_process){
//                    printf("Parent process done: %s, %s, %s\n", args_token[0], args_token[1], args_token[2]);
                }else{
//                    printf("Parent process waiting: %s, %s, %s\n", args_token[0], args_token[1], args_token[2]);
                    waitpid(pid, NULL, 0);
//                    printf("Parent process done: %s, %s, %s\n", args_token[0], args_token[1], args_token[2]);
                }

            }
            // clean args
            int j;
            for(j = 0; j<HALF_LINE; j++){
                args_token[j] = NULL;
            }
        }


    }

return 0;
}



/** After reading user input, the steps are:
 * (1) fork a child process using fork()
 * (2) the child process will invoke execvp()
 * (3) parent will invoke wait() unless command included &
 */
