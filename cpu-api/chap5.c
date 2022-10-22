#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
void problemOne()
{
    int x;
    x = 5;

    int rc = fork();
    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
    }
    else if (rc == 0)
    {
        printf("hello\n");
        printf("%d\n", x);
        x = 88;
    }
    else
    {
        // parent goes down this path (main)
        wait(NULL);
        printf("goodbye\n");
        printf("%d\n", x);
    }
}

// Seems like the parent thread always executes first on the shared file?
void problemTwo()
{

    // open a file
    FILE *ptr;
    ptr = fopen("/Users/dboures/dev/ostep-homework/cpu-api/scratch.txt", "w");

    int rc = fork();
    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
    }
    else if (rc == 0)
    {
        fprintf(ptr, "Child wrote this line\n");
    }
    else
    {
        fprintf(ptr, "Daddy wrote this line\n");
    }
}

void problemThree()
{

    int rc = fork();
    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
    }
    else if (rc == 0) // Child path
    {
        printf("hello\n");
        char *myargs[2];
        myargs[0] = strdup("ls");
        myargs[1] = NULL;
        execvp(myargs[0], myargs);
    }
    else
    { // parent path
        printf("goodbye\n");
    }
}

void problemFour()
{
    int rc = fork();
    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
    }
    else if (rc == 0)
    {
        printf("hello\n");
        int x = wait(NULL); // returns -1
        printf("%d\n", x);
    }
    else
    {
        // parent goes down this path (main)
        int y = wait(NULL);
        printf("goodbye\n");
        printf("%d\n", y);
    }
}

void problemFive()
{
    int rc = fork();
    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
    }
    else if (rc == 0)
    {
        wait(NULL);
        printf("hello\n");
    }
    else
    {
        // parent goes down this path (main)
        printf("goodbye\n");
    }
}

void problemSix()
{
    int rc = fork();
    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
    }
    else if (rc == 0)
    {
        printf("hello\n");
    }
    else
    {
        // parent goes down this path (main)
        int *goo = NULL;
        waitpid(79610, goo, 0);
        printf("goodbye\n");
    }
}

void problemSeven()
{
    int rc = fork();
    if (rc < 0)
    {
        // fork failed
        fprintf(stderr, "fork failed\n");
    }
    else if (rc == 0)
    {
        fclose(stdout);
        printf("hello\n");
    }
    else
    {
        // parent goes down this path (main)
        int *goo = NULL;
        waitpid(79610, goo, 0);
        printf("goodbye\n");
    }
}

void problemEight()
{
    int pipefd[3];
    char ba;
    char bb;

    pipe(pipefd);
    int n1 = fork();
    int n2 = fork();
    if (n1 > 0 && n2 > 0) {
        printf("parent\n");
        close(pipefd[0]);
        write(pipefd[1], "L", strlen("L"));
        close(pipefd[1]);
        wait(NULL);
        
    }
    else if (n1 == 0 && n2 > 0)
    {
        printf("First child\n");
        read(pipefd[0], &ba, 1);
        write(pipefd[2], &ba, 1);
        
        close(pipefd[2]);
    }
    else if (n1 > 0 && n2 == 0)
    {
        printf("Second child\n");
        read(pipefd[2], &bb, 1);     
        printf("%c", bb);
        
        close(pipefd[2]);
    }
    else {
    }
}

int main()
{
    problemEight();
    return 0;
}