#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
#include <time.h>
#include <string.h>

struct pollfd q_interrrupt = {STDIN_FILENO, POLLIN | POLLPRI};

void interrupt(int interval)
{
    char *path = "/proc/interrupts";
    FILE *fd = fopen(path, "r");
    if (fd == NULL)
    {
        perror("");
        return;
    }
    char temp[1000];
    int noOfCpus = 0;
    fscanf(fd, "%[^\n]", temp);
    fclose(fd);
    char *ptr = strtok(temp, " ");
    while (ptr)
    {
        printf("%s\t", ptr);
        noOfCpus++;
        ptr = strtok(NULL, " ");
    }
    printf("\n");

    time_t current, initital = time(NULL) - interval;
    char c;
    while (1)
    {
        if (current - initital >= interval)
        {
            fd = fopen(path, "r");
            if (fd)
            {
                fseek(fd, fscanf(fd, "%[^\n]", temp), SEEK_CUR);
                fseek(fd, fscanf(fd, "%[^\n]", temp), SEEK_CUR);
                fscanf(fd, "%s", temp);
                for (int i = 0; i < noOfCpus; i++)
                {
                    fscanf(fd, "%s", temp);
                    printf("%s\t", temp);
                }
                printf("\n");
                fclose(fd);
            }
            else
            {
                perror("");
            }

            initital = time(NULL);
        }

        if (poll(&q_interrrupt, 1, 1))
        {
            scanf("%c", &c);
            if (c == 'q')
                break;
        }
        current = time(NULL);
    }
}

void dirty(int interval)
{
    char *path = "/proc/meminfo";
    FILE *fd = fopen(path, "r");
    if (fd == NULL)
    {
        perror("");
        return;
    }
    fclose(fd);
    char temp[1000], *ptr;
    int place = 49;
    long long int dirty;
    time_t current, initital = time(NULL) - interval;
    char c;
    while (1)
    {
        if (current - initital >= interval)
        {
            fd = fopen(path, "r");
            if (fd)
            {
                for (int i = 0; i < 16; i++)
                {
                    fseek(fd, fscanf(fd, "%[^\n]", temp), SEEK_CUR);
                }
                fscanf(fd, "%s", temp);
                fscanf(fd, "%[^0-9]", temp);
                fscanf(fd, "%[^\n]", temp);
                printf("%s\n", temp);
                fclose(fd);
            }
            else
            {
                perror("");
            }

            initital = time(NULL);
        }

        if (poll(&q_interrrupt, 1, 1))
        {
            scanf("%c", &c);
            if (c == 'q')
                break;
        }
        current = time(NULL);
    }
}

int nightswatch(char *flag, int noOfArg, char **arguments)
{
    if (noOfArg != 2 || flag[0] != 1 || flag[1] != 'n' || (strcmp(arguments[1], "interrupt") != 0 && strcmp(arguments[1], "dirty") != 0))
    {
        printf("Invalid input format for nightswatch\n");
        return 0;
    }
    int interval = 0, p = 1;
    for (int i = strlen(arguments[0]) - 1; i >= 0; i--)
    {
        interval += (arguments[0][i] - '0') * p;
        p *= 10;
    }

    int pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        if (strcmp(arguments[1], "interrupt"))
        {
            dirty(interval);
        }
        else
        {
            interrupt(interval);
        }
        exit(0);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
    return 0;
}