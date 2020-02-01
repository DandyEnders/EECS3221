#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#define MAX_LINE 80 /* The maximum length command */
#define HALF_LINE MAX_LINE/2 + 1
#define N_CMD_TOKENS 4
#define CMD_LEN 20

/*

    Author : Jinho Hwang
    Project description : https://github.com/hamzehkhazaei/EECS3221_MiniProj1/blob/master/Project_Description_and_Grading.md

    Objective:
    0. Make makefile
    1. Creating the child process and executing the command in the child
    2. Providing a history feature
    3. Adding support of input and output redirection
    4. Allowing the parent and child processes to communicate via a pipe

*/

char **my_tokenizer(char *a_input, char* separator){
    int i = 0;

    // make copy of a_input. Don't touch argument.
    char *input = malloc(sizeof(a_input));
    strcpy(input, a_input);

    // argument tokens will have N_CMD_TOKENS many tokens.
    char **arg_tokens = malloc (N_CMD_TOKENS * sizeof(char *));
    // for(i = 0; i < N_CMD_TOKENS; i++){
    //     arg_tokens[i] = malloc(CMD_LEN * sizeof(char *));
    // }

    // Check if malloc failed
    if (arg_tokens == NULL){
        fprintf(stderr, "malloc failed in function get_args_tokens.\n");
    }

    char *word;         // token word (temp)
    i = 0;              // index to put in arg_tokens

    // Tokenize input
    word = strtok(input, separator);
    while(word != NULL){
        arg_tokens[i] = word;
        word = strtok(NULL, separator);
        i = i + 1;
    }
    arg_tokens[i] = NULL;
    return arg_tokens;
}

char **get_args_tokens(char *input){
    char *sep = " \n";  // separator for tokens
    return my_tokenizer(input, sep);
}


int my_strcmp(char* string1, char* string2){
    if(strcmp(string1, string2) == 0){
        return 1;
    }else{
        return 0;
    }
}

int main(void) {
    // Declaration
    char*   args;           // current args
    char*   prev_args;      // previous args
    char**  args_token;     // tokenized args, read only
    char**  commands;       // tokenized args, mutable

    char    curr_dir[2048]; // pwd

    int     is_background_task; // is next task background?
    int     i;                  // variant    

    // Init
    prev_args = malloc (MAX_LINE * sizeof(char *));
    //args = malloc (MAX_LINE * sizeof(char *)); args is variable. capped at MAX_LINE

    // Loop
    while (1){

        /*
            Start init
        */
        // get curr dir right now.
        getcwd(curr_dir, sizeof(curr_dir));
        is_background_task = 0;

        // Jinho shell, jsh
        printf("jsh:%s$ ", curr_dir);
        fflush(stdout);

        // fresh malloc
        args = malloc (MAX_LINE * sizeof(char *));

        /*
            User Input
        */
        // read a line of input, let it be args
        fgets(args, MAX_LINE, stdin);

        // tokenize the input args
        args_token = get_args_tokens(args);
        
        /*
            Input processing
        */
        // if there is nothing, just go back.
        if (!args_token[0]){
            free(args);
            free(args_token);
            continue;
        }

        // execute history = !!
        if (my_strcmp(args_token[0], "!!")){
            if (!prev_args){ // when prev_args does not exist,
                printf("No commands in history.");
                continue;
            }else{ // When prev_args exists,
                // args = prev_args
                strcpy(args, prev_args);
                args_token = get_args_tokens(args);
                // and go on with execution
            }
            
        }else{
            // When !! was not fired,
            // prev_args will be args now.
            strcpy(prev_args, args);
        }

        // Execute exit, pwd, cd
        if(my_strcmp(args_token[0], "exit")){
            printf("Bye!\n");
            return(0);
        }

        /* 
            Mutable commands creation
        */
        //commands is mutable args_token.
        commands = get_args_tokens(args);

        // Check if background task.
        // Start from last commands.
        // We don't check the first word.
        for(i = N_CMD_TOKENS - 1; i >= 1; i-- ){
            if(commands[i] != NULL){
                int is_last_ampersand = commands[i][strlen(commands[i])-1] == '&';
                is_background_task = is_last_ampersand;
                
                if(is_background_task){
                    // Remove ampersand. Make sure the command slot is null
                    // if ampersand is last character, else just remove ampersand.
                    if(strlen(commands[i]) == 1){
                        commands[i] = NULL;
                    }else{
                        commands[i][strlen(commands[i])-1] = '\0';
                    }
                }
                
                // We only have to check the last non-null word if ampersand exists.
                break;
            }
        }
        
        /*
            Actual execution.
        */
        // Forking
        pid_t pid;
        int status;
        pid = fork();
        
        if(pid < 0){
            fprintf(stderr, "Failed to fork, pid: %d\n", pid);
        }else if(pid == 0){ // if pid == 0, child process.
            if(is_background_task){
                setpgid(0,0); // make this child go background process
            }

        // if not then execution mode
        }else{
            int my_pipe[2];

            // Forking
            pid_t pid;
    //        int status;
            pid = fork();
            //if pid < 0, error occured.
            if(pid < 0){
                exit(1);
            // if pid == 0, child process.
            }else if(pid == 0){
//                printf("Child process: %s, %s, %s\n", commands[0], commands[1], commands[2]);

                execvp(commands[0], commands);
            // if pid > 0, parent process.
            }else{
                if(execvp(commands[0], commands) < 0){
                    fprintf(stderr, "Unknown command: %s\n", commands[0]);
                }
            }
            
        }else{  // if pid > 0, parent process.
            if(is_background_task){
                // nothing...
            }else{
                waitpid(pid, NULL, 0);
            }

        }
        // free memory
        free(args);
        free(args_token);
        free(commands);  

    }

return 0;
}



/** After reading user input, the steps are:
 * (1) fork a child process using fork()
 * (2) the child process will invoke execvp()
 * (3) parent will invoke wait() unless command included &
 */
