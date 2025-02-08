#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

 /*  
 The code captured below required a lot of Internet research on my part. Although I now understand 
 its implementation and why each part is necessary, its construction took a lot of looking at examples 
 and pulling from other (somewhat) similar implementations. I also very much thank the Linux man pages 
 for helping me understand the functions I utilized.
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{

char *commands[CMD_MAX];
char *each_command;
int command_count = 0;
int i;
int u;

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

for (i = 0; i < command_count; i++){
	
    /* 
    This while-loop trims the leading spaces of each token.  
    */
	while (*commands[i] == SPACE_CHAR){
		commands[i]++;
		}
	
    /*  
    Further "tokening" the commands, now separating them with spaces. 
    */    
	char *arg = strtok(commands[i], SPACE_STRING);
	command_t *command2 = &clist->commands[i];
    /*  
    This "exec_found" variable isn't all that useful (and I know in another implementation I could
    likely get rid of it), but in my if-statement below, I use it for my boolean condition so that it
    only runs once (because once it is increment from 0 to 1, it exits) -- but the first part of that 
    condition is crucial: it only runs if the strtok() DOESN'T return NULL (but that could also be 
    true for the arguments, hence the rather pointless executable incrementer as an additional con-
    dition).
    */
	int exec_found = 0;

    if (strlen(arg) > (EXE_MAX - 1)){
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }

    if (arg != NULL && exec_found == 0){

        strncpy(command2->exe, arg, EXE_MAX - 1);
        command2->exe[EXE_MAX - 1] = '\0';
        exec_found++;


    }

    /*  
    It was through researching a solution for this assignment that I learned that setting an array 
    to empty with the null terminator ('\0') was prudent if it is to be handled like a string -- and 
    if it turns out that it is empty and no arguments are to be printed, the program is aware that:
    one, it is a string; and two, that it is empty. After implementing this change my program began 
    passing the test.sh hurdles.   
    */
    command2->args[0] = '\0';

    while ((arg = strtok(NULL, " ")) != NULL){

        while (*arg == SPACE_CHAR){
            arg++;
        }

        /*  
        If-statement handling arguments that are too long (which is set at an EXTREMELY large number).
        */
        if (strlen(command2->args) + (strlen(arg) + 1) > (ARG_MAX-1)){
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        else { 
            if (strlen(command2->args) > 0) {
                
                /* 
                I ended up using the strcat() function to add spaces between my arguments. 
                After speaking with Professor Lilley, I don't believe this is truly necessary, 
                but when I was putting this portion of the code together, I found myself in 
                a position where this was the only way I could imagine solving this just-enough-spaces
                problem (and, at least judging by the output, it does in fact work).
                */
                strcat(command2->args, " ");
            }
            
        strcpy(command2->args + strlen(command2->args), arg);
        }
    

    }
    command2->args[ARG_MAX - 1] = '\0';

}
    
printf(CMD_OK_HEADER, clist->num);

for (u = 0; u < clist->num; u++){

    /*  
    Here are the print statements (as long as no errors were produced in the process of parsing the
    command-line input). If arguments are present, they are printed in brackets after the executable,
    separated by spaces; if there are no arguments, only the executables (or singular executable) is 
    printed. 
    */
    command_t *command3 = &clist->commands[u];
    if (strlen(command3->args) > 0){
        printf("<%d> %s [%s]\n", u + 1, command3->exe, command3->args);
    }
    else {
        printf("<%d> %s\n", u + 1, command3->exe);
    }

}

    return OK;  
}