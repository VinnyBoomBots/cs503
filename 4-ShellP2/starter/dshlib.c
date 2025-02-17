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

extern void print_dragon();

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

int alloc_cmd_buff(cmd_buff_t *cmd_buff){


    cmd_buff->_cmd_buffer = malloc (SH_CMD_MAX);

    if (cmd_buff->_cmd_buffer == NULL){
        return ERR_MEMORY;
    }

    else {
        return OK;
    }

}
/*  
This functionality took me a while to figure out was necessary. But you have to free all buffers --
no matter what form they take!
*/
int free_cmd_buff(cmd_buff_t *cmd_buff){

    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);
    }
    return OK;

}

/*  
I will be honest: this is not a graceful implementation. I tried for hours hoping to get this 
right. I would not be surprised if there are unnecessary steps and/or variables in it -- but 
thank goodness it finally passed the bats tests!
*/
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff){


    int argument_count = 0;

    cmd_buff->argc = 0;
    cmd_buff->argv[0] = '\0';

    bool in_quotes = false;

    strcpy(cmd_buff->_cmd_buffer, cmd_line);

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }

    /*  
    I admit: new_buff and second_new_buff will not win any awards for variable-name descriptiveness.
    */
    char *new_buff = cmd_buff->_cmd_buffer;
    while (*new_buff) {

        if (argument_count == CMD_MAX){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

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

int exec_cmd(cmd_buff_t *cmd){

    pid_t pid;
    int status;

    /*  
    This implementation is more or less taken from the demo. 
    */
    pid = fork();
    if (pid < 0) {
        perror("Error in fork attempt");
        return -1;
    } 
    else if (pid == 0) {  
        if (execvp(cmd->argv[0], cmd->argv) < 0) {
            perror("File not found");
            return-1;
        }
    } 
    else {  
        wait(&status);  
    }
    return 0;

}
 
int exec_local_cmd_loop()
{

    char *cmd_buff = malloc(SH_CMD_MAX);
    int rc = 0;
    cmd_buff_t cmd;

    /*  
    My added variables
    */
    int all_result;
    int free_result;
    int build_buff;

    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

    while(1){
        
        printf("%s", SH_PROMPT);
                
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }
        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
         
        //IMPLEMENT THE REST OF THE REQUIREMENTS

        char *start_of_buff = cmd_buff;

        while (*start_of_buff == SPACE_CHAR) {
        start_of_buff++;
        }

        if (*cmd_buff == '\0'){
            printf(CMD_WARN_NO_CMD);
            break;
        }

        /*  
        Trailing and leadings spaces: how you haunt me. This is a bit of a warm-up for the 
        real work of the build_cmd_buff function -- but I hoped to trim the ends off the ini-
        tial buffer before it was parsed. 
        */
        char *end_of_buff; 

        end_of_buff = cmd_buff + strlen(cmd_buff) - 1;

        while (end_of_buff > cmd_buff && *end_of_buff == SPACE_CHAR){ 
            end_of_buff--;
        }

        *(end_of_buff + 1) = '\0';

        all_result = alloc_cmd_buff(&cmd);

        if (all_result == ERR_MEMORY){
            printf("Memory allocation failed!\n");
            return 99;
        }

        build_buff = build_cmd_buff(cmd_buff, &cmd);

        if (build_buff == ERR_CMD_OR_ARGS_TOO_BIG){
            printf("Too many arguments\n");
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
         
            

        rc = match_command(cmd.argv[0]);

        if (rc == BI_CMD_EXIT){
            free(cmd_buff);
            return OK;
        }

        else if (rc == BI_CMD_DRAGON){
            print_dragon();
        }

        else if (rc == BI_CMD_CD){

            
            if (cmd.argc > 1) {
                int cd = chdir(cmd.argv[1]);
                if (cd != 0) {
                    perror("cd failed");
                    return OK;
                }
                
            }

        }

        else if (rc == BI_NOT_BI){

            int fork_exec_result = exec_cmd(&cmd);

            if (fork_exec_result == -1){
                return ERR_EXEC_CMD;
            }

        }


        free_result = free_cmd_buff(&cmd);

        if (free_result != 0){
            return ERR_MEMORY;
        }


}

    free(cmd_buff);
    //free(cmd._cmd_buffer);
    return OK;
}

