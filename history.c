#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "tilde.h"
#include "redirection.h"
#include "shell.h"
char histories[20][1000];
int end = 0, total = 0;

void history(int n)
{
    if (n > 20)
    {
        printf("Can't store more than 20 histories\n");
        return;
    }
    if (n > total)
    {
        printf("There are only %d histories\n", total);
        n = total;
    }
    int y = end - n;
    y = (y + 40) % 20;
    while (n--)
    {
        // printf("\n");
        printf("%s\n", histories[y]);
        y++;
        y %= 20;
    }
}

void addToHistory(char *buf)
{
    if (total <= 20)
    {
        total++;
    }
    strcpy(histories[end], buf);
    end++;
    end %= 20;
}

void executeThatCommand(int n)
{
    if (n > total)
    {
        printf("Only %d commands have been executed yet\n", total);
        return;
    }
    if (n > 10)
    {
        printf("k should be less than or equal to 10\n");
        return;
    }
    int y = end - n;
    y = (y + 40) % 20;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    switchToTilde(cwd);
    printf("\033[1;32m%s@%s: \033[1;34m%s\033[1;32m> \033[0m%s\n", user, host, cwd, histories[y]);
    addToHistory(histories[y]);
    char *temp, *r = histories[y];
    int total = 0;
    while (temp = strtok_r(r, ";", &r))
    {
        if (total > 0)
            printf("\n");
        parse_and_execute(temp);
        total++;
    }
    return;
}