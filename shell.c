#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include "shell.h"
#include "driver.h"
#include "takingInput.h"
#include "redirection.h"
#include "externalProcesses.h"
#include "history.h"
#include "jobs.h"




void basicHandler(int sig){
    // printf("\n");
    if(fg_pid){
        kill(fg_pid, sig);
    }
}

void initialize()
{
    signal(SIGINT, basicHandler);
    signal(SIGQUIT, basicHandler);
    signal(SIGTSTP, basicHandler);
    signal(SIGCHLD, handler);
    fg_pid=0;
    original_stdin = dup(STDIN_FILENO);
    original_stdout = dup(STDOUT_FILENO);
    readlink("/proc/self/exe", home, sizeof(home));
    int x = 0;
    for (int i = 0; home[i] != '\0'; i++)
    {
        if (home[i] == '/')
        {
            x = i;
        }
    }
    for (int i = x; home[i] != '\0'; i++)
    {
        home[i] = '\0';
    }
    getlogin_r(user, sizeof(user));
    gethostname(host, sizeof(host));
    printf("\e[1;1H\e[2J");
    printf("-------------------------------------------------------\n");
    printf("NAME: PRANAV GOYAL\n");
    printf("ROLL NO: 2018114014\n");
    printf("WELCOME TO MY SHELL\n");
    printf("-------------------------------------------------------\n");
}

int main()
{
    initialize();
    char input[1000];
    int arrC[4];
    for (int i = 0; i < 100; ++i)
        arguments[i] = malloc(100);
    while (1)
    {
        if (takeInput(input,arrC))
        {
            addToHistory(input);
            char *temp, *r = input;
            int total = 0;
            while (temp = strtok_r(r, ";", &r))
            {
                if (total > 0)
                    printf("\n");
                parse_and_execute(temp);
                total++;
            }
        }else if(arrC[0]){
            if(arrC[1] || arrC[2] || arrC[3]){
                printf("Only up arrows are allowed\n");
            }
            else{
                executeThatCommand(arrC[0]);
            }
        }
    }
    return 0;
}