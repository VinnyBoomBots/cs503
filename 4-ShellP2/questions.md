1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: Simplistically, we use the `fork/execvp` combination of functions in this assignment because we want two functions to run at the same time. We want our shell to continue operating, but we also want a child process to begin at the same time and handle external, namely non-built-in, commands that come in from the command line. From my reading of the `execvp` man page, without a `fork` preceding it, all calling `execvp` would do is hijack our current operation, start a new one, but yet never come back to that operation that never finished. `Fork` has ample value in this assignment, as it creates that child process so that `execvp` doesn't hijack our program, but merely takes over until it finishes -- because then it passes operation back to the parent. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: According to the fork() man page, a -1 is the return value if a fork fails for any reason. Because of that, I put together a conditional statement that states if the return value is anything less than zero (and that return value I stored as a variable called pid), then an error message is printed ("fork failed") and the program is exited. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: From what I understand, the system environment variable in question here is PATH, and execvp() searches the directories listed in PATH for the file supplied on the command line -- and execvp() will search PATH over until it finds the file it's looking for. That is as long as that file exist; if it doesn't, execvp() returns -1, and, at least in this program, an error message is produced along with an exit() call. 

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: We call wait() so that parent process ceases while the child process completes its operations. If we didn't, the parent process would continue on without waiting for the child process to complete. At least from what the man pages state, calling fork() doesn't STOP the parent process, it just CREATES the child process and, for a time, they both run concurrently -- but it is calling wait() that forces the parent to cease until the child has finished. 

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: From what I understand, WEXITSTATUS is a macro that "extracts the exit status" of a child process after it's finished its operations. However, it's apparently only provided when termination of the child process occurred normally (I suppose we can say "as expected"), so it is both a way of retaining information on the child-process termination as well as confirming that termination was successful. 

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: Oh, man, did this give me some trouble; I worked on this aspect of the code for hours. (I don't know about my peers, but there was very little code reuse from last week's assignment for me -- I practically rewrote everything trying to get it to work.) Essentially what it does is treats command-line input as two separate things: either commands separated by whitespaces, and commands caught between quotes. And instead of strtok() (which seemed completely unhelpful for this implementation), I just ran over each portion of the buffer, stopping when I hit either whitespace (for non-quoted args) or quotes (for quoted args), and then copied each argument into the argv[] array. And I certainly understand why it was necessary -- quoted strings are just that, STRINGS, and they are one argument, not multiple ones -- but, man, was it difficult to put together. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: There were a lot of challenges. Number one, I didn't use strtok() (as stated above) -- and I really wish I could have, because it makes things a lot easier. Also, when and how often to increment REALLY tripped me up -- and I will admit that I finally broke down and asked an AI helper for a little bit of advice. It was just a very LONG bit of functionality -- and it became hard to follow at times. But I got through it! I can't tell you how excited I was when I finally passed those tests. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: From what I was able to glean from the Internet sources, signals notify processes of certain events -- and once those signals are received, processes respond by either ceasing temporarily or terminating, depending on the signal. They are different from other forms of IPCs, because they can occur seemingly at any time, and after learning about semaphores and mutatexes in lecture this past week, it is interesting to learn that signals do not have these mechanisms built in -- so it seems as if they don't wait in line like other IPCs may. Signals don't seem to have much patience. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: First we have SIGKILL: the "kill signal" (yikes). Unsurprisingly, SIGKILL is used to "kill" (terminate) a process -- and, unfortunately for your program, there is no ignoring it. 
    Second is SIGTERM: "termination SIGNAL," which is a little less intimidating than the "kill signal." Google actually described it as "a polite way to ask a program to terminate" -- and the only way it seems to differ from SIGKILL is that it can, at times, be ignored. 
    Third is SIGINT, which -- according to a source I read -- is the Ctrl+C command (which I used often troubleshooting this assignment). Google states that SIGINT allows a program "to terminate gracefully" -- but, as it is less intimidating than SIGKILL and SIGTERM, apparently some programs can override SIGINT.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: According to Google, SIGSTOP pauses a program, rather than terminating it (like the signals discussed above). But surprisingly, SIGSTOP cannot be ignored like SIGINT, and I assume that's because we're only PAUSING the program, not KILLING it -- and its easy enough to resume, apparently: we just use the SIGCONT signal. 
