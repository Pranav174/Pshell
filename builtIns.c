#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/dir.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "redirection.h"
#include <fcntl.h>
#include "jobs.h"
#include "tilde.h"

void cd(char *flag, int noOfArg, char *arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed for cd\n");
        return;
    }
    if (noOfArg != 1)
    {
        printf("Only single argument required for cd\n");
        return;
    }
    // printf("trying to cd to: %s\n", arguments);
    if (chdir(arguments) == -1)
    {
        perror("");
    }
}
void echo(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed\n");
        return;
    }
    if (noOfArg)
    {
        printf("%s", arguments[0]);
    }
    else
    {
        printf("No arguments provied");
        return;
    }
    for (int i = 1; i < noOfArg; i++)
    {
        printf(" %s", arguments[i]);
    }
    printf("\n");
}

void pwd(char *flag, int noOfArg)
{
    if (flag[0] != 0 || noOfArg != 0)
    {
        printf("No flags or arguments allowed\n");
        return;
    }
    char temp[500];
    if (getcwd(temp, sizeof(temp)) == NULL)
    {
        perror(" ");
    }
    else
    {
        printf("%s\n", temp);
    }
}

void pinfo(char *flag, int noOfArg, char **arguments)
{
    if (noOfArg > 1 || flag[0])
    {
        printf("No flags allowed. Provide either 0 or 1 argument to the command\n");
        return;
    }
    int pid = 0;
    if (noOfArg)
    {
        int n = strlen(arguments[0]);
        int p = 1;
        for (int i = n - 1; i >= 0; i--)
        {
            pid += (arguments[0][i] - '0') * p;
            p *= 10;
        }
    }
    else
    {
        pid = getpid();
    }
    char procfile[200];
    sprintf(procfile, "/proc/%d/stat", pid);
    int fd = open(procfile, O_RDONLY);
    if (fd == -1)
    {
        printf("Can't access the information of pid: %d. Are you sure a process exists with this pid??\n", pid);
        return;
    }
    char temp[500];
    read(fd, temp, 500);
    close(fd);
    char status, vsize[20];
    int total = 0;
    for (int i = 0; i < 500; i++)
    {
        if (temp[i] == ' ')
        {
            total++;
            if (total == 2)
            {
                status = temp[i + 1];
            }
            if (total == 22)
            {
                total = ++i;
                while (temp[i] != ' ')
                {
                    vsize[i - total] = temp[i];
                    i++;
                }
                vsize[i - total] = '\0';
                break;
            }
        }
    }
    sprintf(procfile, "/proc/%d/exe", pid);
    readlink(procfile, procfile, sizeof(procfile));
    switchToTilde(procfile);
    printf("pid -- %d\n", pid);
    printf("Process Status -- %c\n", status);
    printf("memory -- %s\n", vsize);
    printf("Executable Path -- %s\n", procfile);
}

void clear()
{
    printf("\e[1;1H\e[2J");
}

void b_setenv(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed\n");
        return;
    }
    if (noOfArg == 0 || noOfArg > 2)
    {
        printf("Only 1 or 2 arguments allowed\n");
        return;
    }
    if (noOfArg == 1)
    {
        arguments[1][0] = '\0';
    }
    // printf("setting %s to %s\n", arguments[0], arguments[1]);
    if (setenv(arguments[0], arguments[1], 1) == -1)
    {
        perror("");
    }
}

void b_unsetenv(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed\n");
        return;
    }
    if (noOfArg != 1)
    {
        printf("Require single argument\n");
        return;
    }
    // printf("setting %s to %s\n", arguments[0], arguments[1]);
    if (unsetenv(arguments[0]) == -1)
    {
        perror("");
    }
}

void cronjob(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 3 || noOfArg != 3)
    {
        printf("Cronjob requires 3 arguments\n");
        return;
    }
    int c_done = 0, t_done = 0, p_done = 0;
    int period = 1, till = 1;
    char command[200];
    for (int i = 1; i <= 3; i++)
    {
        if (flag[i] == 'c')
        {
            c_done = 1;
            strcpy(command, arguments[i - 1]);
        }
        else if (flag[i] == 't')
        {
            t_done = 1;
            period = atoi(arguments[i - 1]);
        }
        else if (flag[i] == 'p')
        {
            p_done = 1;
            till = atoi(arguments[i - 1]);
        }
    }
    if (!c_done || !p_done || !t_done)
    {
        printf("Invalid arguments for crontab");
        return;
    }
    int pid = fork();
    addJobToLast(pid, "cronjob");
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        setpgid(0, 0);
        int cycles = till / period;
        while (cycles--)
        {
            sleep(period);
            parse_and_execute(command);
        }
        exit(0);
    }
    // else
    // {
    //     // waitpid(pid, NULL, 0);
    // }
}