#define _GNU_SOURCE

// Run example:
//      pi@raspberrypi: $ sudo ./app
//          Parent: Created child with PID=16593
//          Child: PID=1, Parent PID=0
//          Child exiting
//          Parent exiting

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024) // 1MB stack size for the child process

int child_function(void *arg) 
{
    printf("Child: PID=%ld, Parent PID=%ld\n", (long)getpid(), (long)getppid());
    sleep(5);
    printf("Child exiting\n");

    return 0;
}

int main() 
{
    char *stack;
    char *stack_top;
    int flags = CLONE_NEWPID | SIGCHLD; // Please note the CLONE_NEWPID flag
    pid_t child_pid;

    stack = (char *)malloc(STACK_SIZE);
    if (stack == NULL) 
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    stack_top = stack + STACK_SIZE;

    // Create a new process in a new PID namespace
    child_pid = clone(child_function, stack_top, flags, NULL);

    if (child_pid == -1) 
    {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    printf("Parent: Created child with PID=%ld\n", (long)child_pid);

    // Wait for the child to exit
    if (waitpid(child_pid, NULL, 0) == -1) 
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    free(stack);
    printf("Parent exiting\n");

    return 0;
}