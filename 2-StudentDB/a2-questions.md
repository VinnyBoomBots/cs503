## Assignment 2 Questions

#### Directions
Please answer the following questions and submit in your repo for the second assignment.  Please keep the answers as short and concise as possible.

1. In this assignment I asked you provide an implementation for the `get_student(...)` function because I think it improves the overall design of the database application.   After you implemented your solution do you agree that externalizing `get_student(...)` into it's own function is a good design strategy?  Briefly describe why or why not.

    > **Answer**: Externalizing the `get_student(...)` function is certainly a good design strategy,
    as it implements code reuse and eliminates the need for an unnecessary repetition of functionality. 
    I used the `get_student(...)` function in my `add_student(...)` function as well as in my 
    `delete_student(...)` function, and it was also utilized in the `print_student(...)`-function functionality within the provided switch statement. Within my source code, the `get_student(...)` function does the the heavy lifting of finding specific students under different cirumstances, which is a huge aspect of the implementation of my database's functionality. 

2. Another interesting aspect of the `get_student(...)` function is how its function prototype requires the caller to provide the storage for the `student_t` structure:

    ```c
    int get_student(int fd, int id, student_t *s);
    ```

    Notice that the last parameter is a pointer to storage **provided by the caller** to be used by this function to populate information about the desired student that is queried from the database file. This is a common convention (called pass-by-reference) in the `C` programming language. 

    In other programming languages an approach like the one shown below would be more idiomatic for creating a function like `get_student()` (specifically the storage is provided by the `get_student(...)` function itself):

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND: return NULL
    student_t *get_student(int fd, int id){
        student_t student;
        bool student_found = false;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found)
            return &student;
        else
            return NULL;
    }
    ```
    Can you think of any reason why the above implementation would be a **very bad idea** using the C programming language?  Specifically, address why the above code introduces a subtle bug that could be hard to identify at runtime? 

    > **ANSWER:** I had to do a bit of research on this one (namely I did two Internet searches, nothing wild), but it seems that by not dynamically allocating memory for the specific student that we're looking for in the Linux file, we are producing a "dangling pointer" because our "student" is a local variable -- and once the function has run, the memory for that local variable is deallocated, so the return value could be, as one source put it, a "garbage value," and that garbage value could have negative consequences for our program later on.  

3. Another way the `get_student(...)` function could be implemented is as follows:

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND or memory allocation error: return NULL
    student_t *get_student(int fd, int id){
        student_t *pstudent;
        bool student_found = false;

        pstudent = malloc(sizeof(student_t));
        if (pstudent == NULL)
            return NULL;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found){
            return pstudent;
        }
        else {
            free(pstudent);
            return NULL;
        }
    }
    ```
    In this implementation the storage for the student record is allocated on the heap using `malloc()` and passed back to the caller when the function returns. What do you think about this alternative implementation of `get_student(...)`?  Address in your answer why it work work, but also think about any potential problems it could cause.  
    
    > **ANSWER:** In this implementation we are at least dynamically allocating memory to our "pstudent" variable, which is good, but the same issue exists that pstudent is still a local variable to the `get_student(...)` function. The memory to pstudent won't be deallocated after the completion of the function, but it still would be wiser to just pass the variable into the function, as the `get_student(...)` function is used to assist other functions within the code, and thus its return value has greater purpose outside `get_student(...)`'s immediate implementation. 


4. Lets take a look at how storage is managed for our simple database. Recall that all student records are stored on disk using the layout of the `student_t` structure (which has a size of 64 bytes).  Lets start with a fresh database by deleting the `student.db` file using the command `rm ./student.db`.  Now that we have an empty database lets add a few students and see what is happening under the covers.  Consider the following sequence of commands:

    ```bash
    > ./sdbsc -a 1 john doe 345
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 128 Jan 17 10:01 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 3 jane doe 390
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 256 Jan 17 10:02 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 63 jim doe 285 
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 64 janet doe 310
    > du -h ./student.db
        8.0K    ./student.db
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 4160 Jan 17 10:03 ./student.db
    ```

    For this question I am asking you to perform some online research to investigate why there is a difference between the size of the file reported by the `ls` command and the actual storage used on the disk reported by the `du` command.  Understanding why this happens by design is important since all good systems programmers need to understand things like how linux creates sparse files, and how linux physically stores data on disk using fixed block sizes.  Some good google searches to get you started: _"lseek syscall holes and sparse files"_, and _"linux file system blocks"_.  After you do some research please answer the following:

    - Please explain why the file size reported by the `ls` command was 128 bytes after adding student with ID=1, 256 after adding student with ID=3, and 4160 after adding the student with ID=64? 

        > **ANSWER:** The file size fluctates in line with where each record is stored in the file. In our source code, the starting place of each record is equal to its ID number multiplied by the size of each record (i.e., 64 bytes), and since 0 is not within the range of possible ID numbers, the first possible record location is for ID 1 -- starting at the 64-byte location in the file and  (for 64 bytes) to 128-byte location. The same logic holds for ID 3 -- the record begins at file location 192 and extends to location 256 -- and ID 64 -- starting at location 4096 (64 * 64 = 4096) and extending to location 4160.  

    -   Why did the total storage used on the disk remain unchanged when we added the student with ID=1, ID=3, and ID=63, but increased from 4K to 8K when we added the student with ID=64? 

        > **ANSWER:** According to the cursory research I completed, I found that block sizes are typically a power of 2. 4K is specifically 4,096 bytes, which would represent that last byte of the record with an ID of 63. For ID 64, that record would span from 4,096 to 4,160, necessitating an expansion of the block size to the next power, namely 2 to the 13th, which equals 8,192 (or simplified as 8K).

    - Now lets add one more student with a large student ID number  and see what happens:

        ```bash
        > ./sdbsc -a 99999 big dude 205 
        > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 6400000 Jan 17 10:28 ./student.db
        > du -h ./student.db
        12K     ./student.db
        ```
        We see from above adding a student with a very large student ID (ID=99999) increased the file size to 6400000 as shown by `ls` but the raw storage only increased to 12K as reported by `du`.  Can provide some insight into why this happened?

        > **ANSWER:**  Well the `du` command provides an estimate of the file-space usage that our records are taking up. Although the recod for a student with an ID of 99,999 is stored 64,000 bytes into the file, the file-space usage itself is not increasing to that same amount, as most of the space is just taken up by zeroes representing empty records. So we're *storing* records further along in our file but we're not actually utilizing all the space in between, thus the file size is increasing but the file-space usage is not.  