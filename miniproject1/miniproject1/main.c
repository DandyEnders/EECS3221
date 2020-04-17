#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#define MAX_LINE 80 /* The maximum length command */
#define HALF_LINE MAX_LINE/2 + 1
#define N_CMD_TOKENS 10
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

    pid_t   pid;            // for forking
    pid_t   pipe_pid;       // for piping

    int     is_background_task; // is next task background?
    int     is_direct_left;     // cmd < cmd
    int     is_direct_right;    // cmd > cmd
    int     is_pipe;            // cmd | cmd

    int     i;                  // variant  
    int     separator_pos;      // when we direct or pipe, we need to konw the position of direct / pipe


    // Init
    prev_args = malloc (MAX_LINE * sizeof(char *));
    //args = malloc (MAX_LINE * sizeof(char *)); args is variable. capped at MAX_LINE

    // Loop
    while (1){

        /*
            Start init
        */
        is_direct_left = 0;
        is_direct_right = 0;
        is_pipe = 0;
        is_background_task = 0;
    
        i = 0;
        separator_pos = 0;

        // get curr dir right now.
        getcwd(curr_dir, sizeof(curr_dir));

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
            return 0;
        }

        if(my_strcmp(args_token[0], "cd")){
            if(chdir(args_token[1]) != 0){
                fprintf(stderr, "cd failed. dir: %s\n", args_token[1]);
            }
            free(args);
            free(args_token);
            continue;
        }

        /* 
            Mutable commands creation
        */
        //commands is mutable args_token.
        commands = get_args_tokens(args);

        /*
            Check if above commands is background task
        
            Start from last commands.
            We don't check the first word.
        */
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
                i = 0; // break;
            }
        }

        /*
         Check for
         1. left redirection
         2. right redirection
         3. pipe
        */
       for(i = N_CMD_TOKENS - 1; i >= 1; i-- ){
            if(args_token[i] != NULL){
                if(my_strcmp(args_token[i], "<")){
                    is_direct_left = 1; 
                    separator_pos = i;
                    i = 0; // break
                }else if(my_strcmp(args_token[i], ">")){
                    is_direct_right = 1;
                    separator_pos = i;
                    i = 0; // break
                }else if(my_strcmp(args_token[i], "|")){
                    is_pipe = 1; 
                    separator_pos = i; 
                    i = 0; // break
                }
                
            }
        }
        

        /*
            Forking process
        */
        // Forking
        pid = fork();
        
        if(pid < 0){
            fprintf(stderr, "Failed to fork, pid: %d\n", pid);
        }else if(pid == 0){ // if pid == 0, child process.

            /*
               Send child process background
            */
            if(is_background_task){
                setpgid(0,0); // make this child go background process
            }

            /*
                Piping
            */
            if (is_pipe) {
                int my_pipe[2];

                if (pipe(my_pipe) < 0){
                    fprintf(stderr, "Piping failed!\n");
                    return 1;
                }

                pipe_pid = fork();

                if (pipe_pid < 0){
                    fprintf(stderr, "Pipe subprocessing failed!\n");
                    return 1;
                }else if(pipe_pid == 0){
                    close(my_pipe[0]);      // close read pipe
                    dup2(my_pipe[1], 1);    // writepipe = stdout
                    close(my_pipe[1]);      // close it

                    // ls -l | grep lol => ls -l (null) (null)
                    for(i = separator_pos; i < N_CMD_TOKENS; i++){
                        commands[i] = NULL;
                    }

                    if(execvp(commands[0], commands) < 0){
                        fprintf(stderr, "Pipe subprocess: Failed to execute: %s\n", commands[0]);
                        return 1;
                    }
                    
                    return 0;
                }else{ // "parent"
                    close(my_pipe[1]);      // close write pipe
                    dup2(my_pipe[0], 0);    // pipe = stdin
                    waitpid(pipe_pid, NULL, 0); // wait for pipe_pid
                    close(my_pipe[0]);

                    // ls -l | grep lol => grep lol (null) (null) (null)
                    for(i = 0; i < N_CMD_TOKENS - separator_pos ; i++){
                        commands[i] = commands[i+separator_pos+1];
                        commands[i+separator_pos+1] = NULL;
                    }

                    if(execvp(commands[0], commands) < 0){
                        fprintf(stderr, "Pipe subprocess: Failed to execute: %s\n", commands[0]);
                        return 1;
                    }
                    return 0;
                }

                
            }else{ // !is_pipe
                /*
                    Redirection
                */
                if (is_direct_left) {
                    int fd0 = open(args_token[separator_pos + 1], O_RDONLY);
                    if (fd0 == -1) {
                        fprintf(stderr, "Left redirection failed!\n");
                        return 1;
                    }
                    dup2(fd0, 0);
                    close(fd0);
                }else if (is_direct_right){
                    int fd1 = open(args_token[separator_pos + 1], O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR);
                    if (fd1 == -1) {          
                        fprintf(stderr, "Right redirection failed!\n");
                        return 1;
                    }
                    dup2(fd1, 1);
                    close(fd1);
                }

                // make redirection and everything to right null.
                // ls -l > out.txt => ls -l (null) (null)
                if(is_direct_left || is_direct_right){
                    for(i = separator_pos; i < N_CMD_TOKENS; i++){
                        commands[i] = NULL;
                    }
                }

                if(my_strcmp(commands[0], "pwd")){
                    getcwd(curr_dir, sizeof(curr_dir));
                    printf("%s\n", curr_dir);
                }else{
                    if(execvp(commands[0], commands) < 0){
                        fprintf(stderr, "Unknown command: %s\n", commands[0]);
                    }
                }
            }
            return 0;
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