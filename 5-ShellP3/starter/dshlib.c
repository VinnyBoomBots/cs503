#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

/*  
  Did a copy-and-paste job from last week's assignment to retain the print_dragon() function.
*/
 void print_dragon(){

    puts("                                                                        @%%%%                       ");
    puts("                                                                     %%%%%%                         ");
    puts("                                                                    %%%%%%                          ");
    puts("                                                                 % %%%%%%%           @              ");
    puts("                                                                %%%%%%%%%%        %%%%%%%           ");
    puts("                                       %%%%%%%  %%%%@         %%%%%%%%%%%%@    %%%%%%  @%%%%        ");
    puts("                                  %%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%%%%%%%%%%%%          ");
    puts("                                %%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%% %%%%%%%%%%%%%%%           ");
    puts("                               %%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%     %%%            ");
    puts("                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%@ @%%%%%%%%%%%%%%%%%%        %%            ");
    puts("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%                ");
    puts("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              ");
    puts("                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%@              ");
    puts("      %%%%%%%%@           %%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%      %%                ");
    puts("    %%%%%%%%%%%%%         %%@%%%%%%%%%%%%           %%%%%%%%%%% %%%%%%%%%%%%      @%                ");
    puts("  %%%%%%%%%%   %%%        %%%%%%%%%%%%%%            %%%%%%%%%%%%%%%%%%%%%%%%                        ");
    puts(" %%%%%%%%%       %         %%%%%%%%%%%%%             %%%%%%%%%%%%@%%%%%%%%%%%                       ");
    puts("%%%%%%%%%@                % %%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%                     ");
    puts("%%%%%%%%@                 %%@%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  ");
    puts("%%%%%%%@                   %%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              ");
    puts("%%%%%%%%%%                  %%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%  ");
    puts("%%%%%%%%%@                   @%%%%%%%%%%%%%%         %%%%%%%%%%%%@ %%%% %%%%%%%%%%%%%%%%%   %%%%%%%%");
    puts("%%%%%%%%%%                  %%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%% %%%%%%%%%");
    puts("%%%%%%%%%@%%@                %%%%%%%%%%%%%%%%@       %%%%%%%%%%%%%%     %%%%%%%%%%%%%%%%%%%%%%%%  %%");
    puts(" %%%%%%%%%%                  % %%%%%%%%%%%%%%@        %%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%%%%%%%% %%");
    puts("  %%%%%%%%%%%%  @           %%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%% ");
    puts("   %%%%%%%%%%%%% %%  %  %@ %%%%%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%% ");
    puts("    %%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%           @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%% ");
    puts("     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              %%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%   ");
    puts("      @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%               ");
    puts("        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%%%%%%  %%%%%%%          ");
    puts("           %%%%%%%%%%%%%%%%%%%%%%%%%%                           %%%%%%%%%%%%%%%  @%%%%%%%%%         ");
    puts("              %%%%%%%%%%%%%%%%%%%%           @%@%                  @%%%%%%%%%%%%%%%%%%   %%%        ");
    puts("                  %%%%%%%%%%%%%%%        %%%%%%%%%%                    %%%%%%%%%%%%%%%    %         ");
    puts("                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%            ");
    puts("                %%%%%%%%%%%%%%%%%%%%%%%%%%  %%%% %%%                      %%%%%%%%%%  %%%@          ");
    puts("                     %%%%%%%%%%%%%%%%%%% %%%%%% %%                          %%%%%%%%%%%%%@          ");
    puts("                                                                                 %%%%%%%@       ");
 

} 

/*  
  Allocating memory for the _cmd_buffer of the cmd_buff struct. 
*/
int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    cmd_buff->_cmd_buffer = malloc (SH_CMD_MAX);

    if (cmd_buff->_cmd_buffer == NULL){
        return ERR_MEMORY;
    }

    else {
        return OK;
    }
}


/*  
  Taken from my 4_ShellP2 submission.
*/
int free_cmd_buff(cmd_buff_t *cmd_buff)
{

    free(cmd_buff->_cmd_buffer);

    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);
        cmd_buff->argv[i] = NULL;
    }
    return OK;

}

/*  
  Taken from my 4_ShellP2 submission, with a few modifications -- and still it works! Woohoo!
*/
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{

    int argument_count = 0;

    cmd_buff->argc = 0;
    cmd_buff->argv[0] = NULL;

    bool in_quotes = false;

    strcpy(cmd_buff->_cmd_buffer, cmd_line);

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }

    /*  
    I kept the variable names new_buff and second_new_buff to make my life easier (but again, I 
    could have come up with better variable names). 
    */
    char *new_buff = cmd_buff->_cmd_buffer;

    while (*new_buff == SPACE_CHAR) {
        new_buff++;
    }

    while (*new_buff) {

        while (*new_buff == SPACE_CHAR && !in_quotes){
            new_buff++;
        }

        if (*new_buff == '"') {
            in_quotes = !in_quotes;
            new_buff++;
            continue;
        }

        char *second_new_buff = new_buff;
        while (*new_buff && (in_quotes || *new_buff != SPACE_CHAR)) {
            if (*new_buff == '"' && in_quotes) {
                in_quotes = !in_quotes;
                break;
            }
            new_buff++;
        }

        size_t len = new_buff - second_new_buff;
        
        if (len > 0 && argument_count < CMD_ARGV_MAX) {
            cmd_buff->argv[argument_count] = malloc(len + 1);
            strncpy(cmd_buff->argv[argument_count], second_new_buff, len);
            cmd_buff->argv[argument_count][len] = '\0';
            argument_count++;
        }

        if (*new_buff) {
            new_buff++;
        }
    }

    cmd_buff->argc = argument_count;

    return OK; 

}


/*  
  Repurposed from my 3-ShellP1 submission. 
*/
int build_cmd_list(char *cmd_line, command_list_t *clist)
{

    char *commands[CMD_MAX];
    char *each_command;
    int command_count = 0;
    int i;

    each_command = strtok(cmd_line, PIPE_STRING);

    while (each_command != NULL){

        if (command_count >= CMD_MAX) {

            return ERR_TOO_MANY_COMMANDS;
		
        }

	commands[command_count] = each_command;
    /*  
    Calling strtok() again, now with NULL as the first parameter, picking up where the last token
    left off (and running until strtok() returns NULL, signaling the end of the string).
    */
	each_command = strtok(NULL, PIPE_STRING);
    command_count++;

    }

    /*  
    Populating the clist (command_list_t) "num" variable with the number of commands parsed from the 
    command-line input. 
    */
    clist->num = command_count;

    if (clist->num == 0){
        return WARN_NO_CMDS;
    }

  
    for (i = 0; i < clist->num; i++){
        alloc_cmd_buff(&clist->commands[i]);
        /*  
          Building each cmd_buff_t for my command_list_t. (Memory allocation is done above.)
        */
        build_cmd_buff(commands[i], &clist->commands[i]);
    }

    return OK;  

} 

int execute_pipeline(command_list_t *clist)
{

    /*  
      This implementation is taken almost entirely from this week's demo code. 
    */
    int pipes[clist->num - 1][2];
    pid_t pids[clist->num];

    for (int i = 0; i < clist->num - 1; i++){
        if (pipe(pipes[i]) == -1){
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < clist->num; i++){
        pids[i] = fork();
        if (pids[i] == -1){
            return ERR_EXEC_CMD;  
        }
        if (pids[i] == 0){

            if (i > 0){
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            if (i < clist->num - 1){
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < clist->num - 1; j++){
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            } 

            if (execvp(clist->commands[i].argv[0], clist->commands[i].argv) < 0) {
                /*  
                  I added this print statement for commands that do not exist. 
                */
                printf("Command not found\n");
                exit (99);
            }
    
        }
    
    }

    for (int i = 0; i < clist->num - 1; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < clist->num; i++){
        waitpid(pids[i], NULL, 0);
    }

    return OK;

}

Built_In_Cmds match_command(const char *input){

    if (strcmp(input, EXIT_CMD) == 0){
        return BI_CMD_EXIT;
    }

    else if (strcmp(input, "dragon") == 0){
        return BI_CMD_DRAGON;
    }

    else if (strcmp(input, "cd") == 0){
        return BI_CMD_CD;
    }
    
    else {
        return BI_NOT_BI;
    }

}

int exec_local_cmd_loop()
{

    char *cmd_buff = malloc (SH_CMD_MAX);
    int rc = 0;
    command_list_t clist;
    int exec_result;

    /*  
    This memory-allocation failure message is taken directly from the source code provided for Week 
    3's assignment. 
    */
    if (cmd_buff == NULL){
    printf("Memory allocation failed!\n");
    return 99;
    }
   
    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
        printf("\n");
        break;
        }
        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
        
        //IMPLEMENT THE REST OF THE REQUIREMENTS 

        rc = build_cmd_list(cmd_buff, &clist);

        if (rc == ERR_TOO_MANY_COMMANDS){

            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;

        }

        if (rc == WARN_NO_CMDS){

            printf(CMD_WARN_NO_CMD);
            continue;

        }

        if (rc == ERR_CMD_OR_ARGS_TOO_BIG){

            printf("Argument-length limit has been reach.\n");
            continue;

        }

        /*  
          Checking to see if any of the initial commands are built-ins.
        */
        rc = match_command(clist.commands[0].argv[0]);

        if (rc == BI_CMD_EXIT){
            for (int j = 0; j < clist.num; j++){
                free_cmd_buff(&clist.commands[j]);
            }
            free(cmd_buff);
            return OK;
        }

        else if (rc == BI_CMD_DRAGON){
            print_dragon();
            continue;
        }

        else if (rc == BI_CMD_CD){
            
            if (clist.commands[0].argc > 1) {
                int cd = chdir(clist.commands[0].argv[1]);
                if (cd != 0) {
                    printf("cd command failed\n");
                }
            }
            continue;
        }

        else{

            exec_result = execute_pipeline(&clist);

            if (exec_result == ERR_EXEC_CMD){
                printf("Error in fork and pipe execution\n");
            }

            if (exec_result == OK){
                for (int i = 0; i < clist.num; i++) {
                    wait(NULL);
                }
            }  

        }

    }
    
    free(cmd_buff);
    return OK;
}
