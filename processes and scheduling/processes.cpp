#include "src.hpp"

// Fill in this method to complete Homework 1, Part 1
// Method is called by 'parent' process
void run_processes()
{

    // Create the first child process
    __pid_t child_pid = fork();

    if (child_pid > 0)
    {
        // parent proc waits for child process to finish using waitpid
        waitpid(child_pid, NULL, 0);

        // after child is done parent calls its function
        parent();
    }
    else if (child_pid == 0)
    {
        // CHILD PROC
        // child process will now create its own child, the grandchild proc
        pid_t grandchild_pid = fork();

        if (grandchild_pid > 0)
        {
            // we are still in the child procs code block, but we are going to wait for grandchild (our child to finish)
            waitpid(grandchild_pid, NULL, 0);

            // after grandchild is done, the child calls its function
            child();

            exit(0);
        }
        else if (grandchild_pid == 0)
        {
            // grandchild proc
            // grandchild should run its function first
            grandchild();

            // grandchild should exit after it completes
            exit(0);
        }
    }
}
