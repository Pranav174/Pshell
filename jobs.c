#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
// #include <sys/types.h>
#include <signal.h>
#include "externalProcesses.h"
int noOfJobs = 0;
int fg_pid = 0;
struct child
{
    int pid;
    char command[200];
    int stat;
    struct child *next;
};
typedef struct child *job;

job head = NULL;

job createNewIndex()
{
    job temp;
    temp = (job)malloc(sizeof(struct child));
    temp->next = NULL;
    return temp;
}

void addJob(int pid, char *command)
{
    job new = createNewIndex();
    new->next = head;
    new->pid = pid;
    strcpy(new->command, command);
    head = new;
    noOfJobs++;
    return;
}

void addJobToLast(int pid, char *command)
{
    job new = createNewIndex();
    new->pid = pid;
    strcpy(new->command, command);
    if (!head)
    {
        head = new;
    }
    else
    {
        job itr = head;
        while (itr->next)
        {
            itr = itr->next;
        }
        itr->next = new;
    }
    noOfJobs++;
    return;
}

job deleteJobByPid(int pid)
{
    if (head)
    {
        if (head->pid == pid)
        {
            job temp = head;
            head = head->next;
            noOfJobs--;
            return temp;
        }
        job itr = head;
        while (itr->next)
        {
            if (itr->next->pid == pid)
            {
                job temp = itr->next;
                itr->next = temp->next;
                noOfJobs--;
                return temp;
            }
            itr = itr->next;
        }
    }
    return NULL;
}

void printJobs()
{
    job itr = head;
    int count = 1;
    char procfile[200], status[200];
    while (itr)
    {
        sprintf(procfile, "/proc/%d/stat", itr->pid);
        int fd = open(procfile, O_RDONLY);
        if (fd == -1)
        {
            printf("Can't access the STATE information of pid: %d.\n", itr->pid);
            strcpy(status, "UNKNOWN STATE");
        }
        else
        {
            char temp[500];
            read(fd, temp, 500);
            close(fd);
            int total = 0;
            for (int i = 0; i < 500; i++)
            {
                if (temp[i] == ' ')
                {
                    total++;
                    if (total == 2)
                    {
                        if (temp[i + 1] == 'S' || temp[i + 1] == 'R')
                        {
                            strcpy(status, "Running");
                        }
                        else if (temp[i + 1] == 'T')
                        {
                            strcpy(status, "Stopped");
                        }
                        else
                        {
                            status[0] = temp[i + 1];
                            status[1] = '\0';
                        }
                        break;
                    }
                }
            }
        }
        printf("[%d] %s %s [%d]\n", count, status, itr->command, itr->pid);
        count++;
        itr = itr->next;
    }
    return;
}

void kjob(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed for kjob\n");
        return;
    }
    if (noOfArg != 2)
    {
        printf("kjob requires 2 arguments\n");
        return;
    }
    int x = atoi(arguments[0]);
    int y = atoi(arguments[1]);
    if (noOfJobs < x)
    {
        printf("There are only %d jobs in the background currently\n", noOfJobs);
        return;
    }
    int index = 1;
    job itr = head;
    while (index != x)
    {
        if (!itr)
        {
            printf("Error occured while iterating through the jobs\n");
            return;
        }
        index++;
        itr = itr->next;
    }
    if (!itr)
    {
        printf("Error occured while iterating through the jobs\n");
        return;
    }
    if (y == 9)
    {
        itr = deleteJobByPid(itr->pid);
    }
    if (kill(itr->pid, y) == -1)
    {
        perror("");
    }
    else
    {
        printf("Successfully sent signal: %d to process with pid: %d\n", y, itr->pid);
    }
}

void fg(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed for fg\n");
        return;
    }
    if (noOfArg != 1)
    {
        printf("fg requires 1 arguments\n");
        return;
    }
    int x = atoi(arguments[0]);
    if (noOfJobs < x)
    {
        printf("There are only %d jobs in the background currently\n", noOfJobs);
        return;
    }
    int index = 1;
    job itr = head;
    while (index != x)
    {
        if (!itr)
        {
            printf("Error occured while iterating through the jobs\n");
            return;
        }
        index++;
        itr = itr->next;
    }
    if (!itr)
    {
        printf("Error occured while iterating through the jobs\n");
        return;
    }
    // bring it to foreground basically means we have to wait for it
    signal(SIGCHLD, SIG_IGN);
    kill(itr->pid, SIGCONT);
    fg_pid = itr->pid;
    // setpgid(itr->pid, getpid());
    struct child p = *itr;
    itr = deleteJobByPid(itr->pid);
    free(itr);
    int status;
    waitpid(p.pid, &status, WUNTRACED);
    fg_pid = 0;
    if (WIFSTOPPED(status))
    {
        addJobToLast(p.pid, p.command);
    }
    signal(SIGCHLD, handler);
}

void bg(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed for fg\n");
        return;
    }
    if (noOfArg != 1)
    {
        printf("bg requires 1 arguments\n");
        return;
    }
    int x = atoi(arguments[0]);
    if (noOfJobs < x)
    {
        printf("There are only %d jobs in the background currently\n", noOfJobs);
        return;
    }
    int index = 1;
    job itr = head;
    while (index != x)
    {
        if (!itr)
        {
            printf("Error occured while iterating through the jobs\n");
            return;
        }
        index++;
        itr = itr->next;
    }
    if (!itr)
    {
        printf("Error occured while iterating through the jobs\n");
        return;
    }
    if (strcmp(itr->command, "vim") && strcmp(itr->command, "less") && strcmp(itr->command, "man"))
        kill(itr->pid, SIGCONT);
}

void overkill(char *flag, int noOfArg, char **arguments)
{
    if (flag[0] != 0)
    {
        printf("No flags allowed for overkill\n");
        return;
    }
    if (noOfArg != 0)
    {
        printf("No arguments allowed\n");
        return;
    }
    int index = 1;
    job temp, itr = head;
    head = NULL;
    signal(SIGCHLD, SIG_IGN);
    while (itr)
    {
        temp = itr;
        itr = itr->next;
        if (kill(temp->pid, 9) == -1)
        {
            perror("");
        }
        printf("KILLED: %d\n", temp->pid);
        waitpid(temp->pid, NULL, 0);
        free(temp);
    }
    signal(SIGCHLD, handler);
}
