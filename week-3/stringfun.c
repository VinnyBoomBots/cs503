#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// I needed to include <stdbool.h> in order to utilize boolean operators.
#include <stdbool.h>


#define BUFFER_SZ 50

// I declared define statements for the space character and the period character.
#define SPACE_CHAR ' '
#define DOT '.'

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

int mystrlen(char *);



int mystrlen(char *str){
    
    int len = 0; 

    /* 
    I must admit that this function took me quite a lot of time to put together. I 
    really struggled with coming up with the correct formula to remove all the unnecessary 
    white space, and although the homework directions tell us to implement this functionality
    in the setup_buff() function, I did not know how that could be possible -- because 
    the length of the string is needed AFTER the white spaces are removed, NOT before; if the 
    length is calculated before, the buffer would allocate more space than necessary for the 
    now-modified string. Moreover, I have to admit I did a lot of Internet searching in order
    to construct an appropriate functon for the white-space-removal process -- so I cannot take
    full credit and say that I alone put this together. However, I am proud of the result.  
    */

    char *old = str;
    char *new = str;

    /*
    I added the if-statement below to account for a situation when the user provides an 
    empty string (i.e., "" with nothing between the double quotes). Per the assignment de-
    scription, I utilized exit(1) to terminated the program, as the issue in question is a 
    command-line problem. 
    */
    if (*old == '\0'){
        printf("Please provide a string\n");
        exit(1);
    }

    while (*old == SPACE_CHAR){
        old++;
    }
    while (*old != '\0'){

        if (*old != SPACE_CHAR){
            *new = *old;
            new++;
            len++;
        }

        else if (*(old + 1) != SPACE_CHAR && *(old +1) != '\0'){
            *new = SPACE_CHAR;
            new++;
            len++;
        }

        old++;

    }
    *new = '\0';

    /* 
    Within this function I was able to construct it so that WITHIN the white-space elimination
    process, I could also obtain the length of the now-modified string (as new and len are
    incremented simultaneously).
    */

    
    return len; 
}


//prototypes for functions to handle required functionality
int  count_words(char *, int);
//add additional prototypes here
void reverse_string(char *, int);
void word_print(char *, int);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    //for now just so the code compiles. 
    
    int str_len;
 
        memset(buff, DOT, len);

        str_len = mystrlen(user_str);

        if (str_len > len){
            printf("The user-supplied string is too large\n");
            return -1;
        }
        
        memcpy(buff, user_str, str_len);
        return str_len;
        
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}
/* 
I decided to remove the "len" parameter from the count_words function, as its utility was 
lacking. If the string provided by the user was larger than the buffer, that would have
already been caught by the setup_buffer() function -- and the program would have already 
terminated with an error message provided. 
*/
int count_words(char *buff, int str_len){
    //YOU MUST IMPLEMENT
    /*
    This function is modeled off the function I constructed for Week 2's assignment.  
    */

    int wc = 0;
    bool word_start;

    int i;

    word_start = false;

    for (i = 0; i < str_len + 1; i++){
        
        if (word_start == false){
            wc++;
            word_start = true;
        }

        else if (word_start == true){
            if (*(buff + i) == SPACE_CHAR || *(buff + i) == DOT)
                word_start = false;
            else 
                continue;
        }

    }

    return wc;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
/* 
The reverse_string() and word_print() functions are also modeled directly from Week 2's 
assignment.  
*/

void reverse_string(char *buff, int str_len){

    int end_idx = str_len - 1;
    int start_idx = 0;
    char tmp_char;

    while (end_idx > start_idx){
        tmp_char = *(buff + end_idx);
        *(buff + end_idx) = *(buff + start_idx);
        *(buff + start_idx) = tmp_char;

        start_idx++;
        end_idx--;
    }

    return;
    
}

void word_print(char *buff, int len){

    int wc = 0;
    int wlen = 0;
    bool word_start = false;

    int i;

    for (i = 0; i < len; i++){

        if (word_start == false){

            if (*(buff + i) == SPACE_CHAR){
                continue;
            }

            else {
                wc++;
                word_start = true;
                wlen = 1;
                printf("%d. ", wc);
                printf("%c", *(buff + i));
            }

        }

        else if (word_start == true){

            if (*(buff + i) != SPACE_CHAR){
                wlen++;
                printf("%c", *(buff + i));
            }

            else{
                printf(" (%d)\n", wlen);
                word_start = false;
                wlen = 0;
            }

        }

    }

    printf(" (%d)\n", wlen);

}



int main(int argc, char *argv[]){

    char *buff = malloc(BUFFER_SZ);             //placehoder for the internal buffer
    /*
    In my declaration of *buff I allocated memory space the size of BUFFER_SZ. Although
    the directions state to do it further down in the code, by doing it this way I was 
    able to rid my console of compiler-supplied warnings.  
    */
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

   

    //TODO:  #1. WHY IS THIS SAFE, aka what if argv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

          /*   Well this if-statement is "safe" because if the user does not
            provide a flag or operation AFTER running the program (namely 
            if there is no argv[1] provided on the command line) the program
            prints out directions instructing the user to run the 
            program (i.e., stringfun), then provide a flag, then finally provide
            a string for the program to utilize (this is the same message displayed 
            if the user had provided the -h (help) flag on the command line). 
            It's a rather elegant way to redirect the user to provide the appropriate 
            commands in the command line.  */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

            /* 
            This if statement is rather elegant as well. If the user provides
            less than required amount of commands on the command line -- namely
            if they provide less than three -- then the -h (help) instructions are
            again printed, instructing the user to provide the requisite amount of argu-
            ments (i.e., program name, flag, then string captured in double quotes). 
            */

    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    /*
    Memory-space allocation for *buff is accomplished above in the variable declarations.   
    */
    
    if (buff == NULL){
        printf("Memory allocation failed\n");
        return 99;
    }
    

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        /*  
        Much of this switch-statement construction is taken from Week 2's starter code. 
        */
        case 'r':
            reverse_string(buff, user_str_len);

            printf("Reversed string: ");

            for (int i = 0; i < user_str_len; i++){
                printf("%c", *(buff + i));
            }

            printf("\n");
            break;    

        case 'w':
            printf("Word Print\n---------------\n");
            word_print(buff, user_str_len);
            break;
        
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
/*  
Provider the length of the buffer as parameters is a good way to cover
your bases, as program code can certainly get remarkably complicated over time and it never 
hurts to reiterate values -- and it is obviously helpful for those READING your code, because it  
allows them to follow your logic in each and every function that you're utilizing. But that said, 
unused parameters trigger warnings from the compiler, and I kind of like getting rid of those warnings
when I "make" my programs: it just looks neater. But for consistency and readable, reminders of 
certain values are not a bad thing -- if anything, as stated above, they are, definitionally speaking, 
good practice.
*/