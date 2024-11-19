#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
int main(int argc, char const *argv[])
{
    int pipe1[2];
    int pipe2[2];
    int pipe3[2];
    pid_t child1_pid;
    pid_t child2_pid;
    if (pipe(pipe1) == -1)
    {
        perror("pipe failed");
        return 1;
    }
    if (pipe(pipe2) == -1)
    {
        perror("pipe failed");
        return 1;
    }
    if (pipe(pipe3) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    // create child that squares input
    child1_pid = fork();
    if (child1_pid < 0)
    {
        perror("Fork failed for child 1");
        return 1;
    }
    if (child1_pid == 0)
    {
        // close unnecessary pipes
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe3[0]);
        close(pipe3[1]);

        // square input from parent and send to child 2
        int squared;
        while (read(pipe1[0], &squared, sizeof(squared)) > 0)
        {
            squared *= squared;
            write(pipe2[1], &squared, sizeof(squared));
        }
        close(pipe1[0]);
        close(pipe2[1]);
        exit(EXIT_SUCCESS);
    }

    // create child that adds one
    child2_pid = fork();
    if (child2_pid < 0)
    {
        perror("Fork failed for child 2");
        return 1;
    }
    if (child2_pid == 0)
    {
        // close unnecessary pipes
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe1[0]);
        close(pipe1[1]);

        // add 1 to input from child 1 and send to parent
        int plus_one;
        while (read(pipe2[0], &plus_one, sizeof(plus_one)) > 0)
        {
            plus_one += 1;
            write(pipe3[1], &plus_one, sizeof(plus_one));
        }
        close(pipe2[0]);
        close(pipe3[1]);
        exit(EXIT_SUCCESS);
    }

    // Parent
    // close unnecessary pipes
    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);
    close(pipe3[1]);

    // read input from keyboard and send to child 1
    int input;
    int out;
    printf("Enter integers:\n");
    while (scanf("%d", &input) != EOF)
    {
        write(pipe1[1], &input, sizeof(input));
        // read output from child 2 and print out
        read(pipe3[0], &out, sizeof(out));
        printf("Result: %d\n", out);
    }
    // close pipes when finished and wait for children to terminate
    close(pipe1[1]);
    close(pipe3[0]);
    waitpid(child1_pid, NULL, 0);
    waitpid(child2_pid, NULL, 0);
    return 0;
}
