#include "shell.h"
#include "builtIns.h"
#include "ls.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tilde.h"
#include "history.h"
#include "nightswatch.h"
#include "jobs.h"

int checkBuiltIn(char *command, char *flag, int noOfArg, char **arguments)
{
    // check if command is equal to some built in and run and return 1 if it is
    if (!strcmp(command, "ls"))
    {
        for (int i = 0; i < noOfArg; i++)
        {
            replaceTilde(arguments[i]);
        }
        parse_ls(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "la"))
    {
        for (int i = 0; i < noOfArg; i++)
        {
            replaceTilde(arguments[i]);
        }
        ls(noOfArg, arguments, 1, 1);
        return 1;
    }
    if (!strcmp(command, "ll"))
    {
        for (int i = 0; i < noOfArg; i++)
        {
            replaceTilde(arguments[i]);
        }
        ls(noOfArg, arguments, 1, 0);
        return 1;
    }
    if (!strcmp(command, "quit"))
    {
        flag[0]=0;
        noOfArg=0;
        overkill(flag, noOfArg, arguments);
        exit(0);
        return 1;
    }
    if (!strcmp(command, "cd"))
    {
        replaceTilde(arguments[0]);
        cd(flag, noOfArg, arguments[0]);
        return 1;
    }
    if (!strcmp(command, "history"))
    {
        if (noOfArg == 0)
        {
            history(10);
        }
        else if (noOfArg == 1)
        {
            int n = 0, p = 1;
            for (int i = strlen(arguments[0]) - 1; i >= 0; i--)
            {
                n += (arguments[0][i] - '0') * p;
                p *= 10;
            }
            history(n);
        }
        else
        {
            printf("History requires either 0 or 1 argument\n");
        }
        return 1;
    }
    if (!strcmp(command, "echo"))
    {
        echo(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "nightswatch"))
    {
        nightswatch(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "pwd"))
    {
        pwd(flag, noOfArg);
        return 1;
    }
    if (!strcmp(command, "clear"))
    {
        clear();
        return 1;
    }
    if (!strcmp(command, "pinfo"))
    {
        pinfo(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "cronjob"))
    {
        cronjob(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "setenv"))
    {
        b_setenv(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "unsetenv"))
    {
        b_unsetenv(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "jobs"))
    {
        if (noOfArg != 0)
        {
            printf("no arguments allowed\n");
            return 1;
        }
        printJobs();
        return 1;
    }
    if (!strcmp(command, "kjob"))
    {
        kjob(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "fg"))
    {
        fg(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "bg"))
    {
        bg(flag, noOfArg, arguments);
        return 1;
    }
    if (!strcmp(command, "overkill"))
    {
        overkill(flag, noOfArg, arguments);
        return 1;
    }
    return 0;
}