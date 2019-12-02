#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include "jobs.h"

void handler(int sig)
{
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status) && pid != -1)
    {
        job temp = deleteJobByPid(pid);
        if (temp)
        {
            // if (status != 98 && status != (98 << 8))
            if (WEXITSTATUS(status) != 98)
            {
                fprintf(stderr, "\n%s with pid %d exited ", temp->command, pid);
                if (WEXITSTATUS(status) == 0)
                {
                    fprintf(stderr, "normally\n");
                }
                else
                {
                    fprintf(stderr, "with status: %d\n", WEXITSTATUS(status));
                }
            }
            free(temp);
        }
    }
    if (WIFSIGNALED(status) && pid != -1)
    {
        job temp = deleteJobByPid(pid);
        if (temp)
        {
            fprintf(stderr, "\n%s with pid %d terminated due to the signal %d", temp->command, pid, WTERMSIG(status));
            free(temp);
        }
    }
}

void background_2(char *command, char *flag, int noOfArg, char **arguments)
{
    signal(SIGCHLD, handler);
    int pid = fork();
    if (pid == -1)
    {
        perror("");
        return;
    }
    else
    {
        addJobToLast(pid, command);
        if (pid == 0)
        {
            if (strcmp(command, "vim") && strcmp(command, "less") && strcmp(command, "man"))
                setpgid(0, 0);
            char *args[2 + noOfArg + 1];
            args[0] = command;
            int j = 0;
            if (flag[0])
            {
                flag[flag[0] + 1] = '\0';
                flag[0] = '-';
                args[1] = flag;
                j = 1;
            }
            for (int i = 0; i < noOfArg; i++)
            {
                args[i + 1 + j] = arguments[i];
            }
            args[j + 1 + noOfArg] = NULL;
            execvp(command, args);
            fprintf(stderr, "Can't execute the command: %s\n", command);
            exit(98);
        }
        else{
            if (!(strcmp(command, "vim") && strcmp(command, "less") && strcmp(command, "man"))){
                kill(pid,SIGSTOP);
            }
        }
    }
}

void foreground(char *command, char *flag, int noOfArg, char **arguments)
{
    signal(SIGCHLD, SIG_IGN);
    int pid = fork();
    if (pid == -1)
    {
        perror("");
        signal(SIGCHLD, handler);
        return;
    }
    else if (pid == 0)
    {
        if (strcmp(command, "vim") && strcmp(command, "less") && strcmp(command, "man"))
            setpgid(0, 0);
        char *args[2 + noOfArg + 1];
        args[0] = command;
        int j = 0;
        if (flag[0])
        {
            flag[flag[0] + 1] = '\0';
            flag[0] = '-';
            args[1] = flag;
            j = 1;
        }
        for (int i = 0; i < noOfArg; i++)
        {
            args[i + 1 + j] = arguments[i];
        }
        args[j + 1 + noOfArg] = NULL;

        execvp(command, args);
        fprintf(stderr, "Can't execute the command: %s\n", command);
        _exit(98);
    }
    else
    {
        // int rn = tcgetpgrp(0);
        // printf("rn:%d\n", rn);
        // tcsetpgrp(0, pid);
        // tcsetpgrp(0,rn);
        // printf("1\n");
        fg_pid = pid;
        int status;
        waitpid(pid, &status, WUNTRACED);
        // printf("2\n");
        // printf("gg%d\n", tcgetpgrp(0));
        // printf("dbfdb%d\n",tcsetpgrp(0, rn));
        // printf("3\n");

        if (WIFSTOPPED(status))
        {
            addJobToLast(pid, command);
        }
        fg_pid = 0;
        signal(SIGCHLD, handler);
    }
}