#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "takingInput.h"
#include "externalProcesses.h"
#include "driver.h"

char command[500], flag[500], *arguments[100], r_stdin[200], r_stdout[200];
int original_stdin, original_stdout, noOfArg;
int pipe_ends[2];
int r_stdin_allowed, r_stdout_allowed;

void checkCommand()
{
    printf("Command : %s\n", command);
    for (int i = 1; i <= flag[0]; i++)
    {
        printf("flag : %c\n", flag[i]);
    }
    for (int i = 0; i < noOfArg; i++)
    {
        printf("argument : %s\n", arguments[i]);
    }
    if (r_stdin[0])
    {
        printf("Std_input file = %s\n", r_stdin + 1);
    }
    else
    {
        printf("No stdinput file\n");
    }

    if (r_stdout[0])
    {
        if (r_stdout[1])
        {
            printf("Std_output file with appending= %s\n", r_stdout + 2);
        }
        else
        {
            printf("Std_output file = %s\n", r_stdout + 2);
        }
    }
    else
    {
        printf("No std output file\n");
    }
    printf("\n");
}

void execute()
{
    // printf("executing: %s\n", command);
    if (!checkBuiltIn(command, flag, noOfArg, arguments))
    {
        if (noOfArg)
        {
            if (strcmp(arguments[noOfArg - 1], "&") == 0)
            {
                noOfArg--;
                background_2(command, flag, noOfArg, arguments);
            }
            else
            {
                foreground(command, flag, noOfArg, arguments);
            }
        }
        else
        {
            foreground(command, flag, noOfArg, arguments);
        }
    }
}

void runInChildProcess(int in, int out)
{
    pid_t pid;

    if ((pid = fork()) == 0)
    {
        if (in != 0)
        {
            dup2(in, 0);
            close(in);
        }
        if (out != 1)
        {
            dup2(out, 1);
            close(out);
        }
        execute();
        _exit(105);
    }
    waitpid(pid, NULL, 0);
    return;
    // else
    // {
    // }
}

int codeRunner(char *input, int last)
{
    int in, out;
    int stat = parseInput(input, command, flag, &noOfArg, arguments, r_stdin, r_stdout);
    // checkCommand();
    if (stat == 1)
    {
        printf("Please complete the \"\n");
        return -1;
    }
    if (stat == 2)
    {
        printf("Invalid Command: No file provided for redirection\n");
        return -1;
    }
    if (r_stdin_allowed)
    {
        if (r_stdin[0] == 1)
        {
            in = open(r_stdin + 1, O_RDONLY);
            if (in == -1)
            {
                perror("");
                return -1;
            }
        }
        else
        {
            in = 0;
        }
    }
    else
    {
        if (r_stdin[0] == 1)
        {
            printf("Invalid command: no input allowed for %s as redirected by pipe\n", command);
            return -1;
        }
        else
        {
            in = pipe_ends[0];
        }
    }
    if (last == 0)
    {
        if ((pipe(pipe_ends)) < 0)
        {
            perror("");
            return -1;
        }
    }
    if (r_stdout_allowed)
    {
        if (r_stdout[0])
        {
            if (r_stdout[1])
            {
                // append write
                out = open(r_stdout + 2, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
                if (out == -1)
                {
                    perror("");
                    return -1;
                }
            }
            else
            {
                // normal write
                out = open(r_stdout + 2, O_WRONLY | O_TRUNC | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
                if (out == -1)
                {
                    perror("");
                    return -1;
                }
            }
        }
        else
        {
            out = 1;
        }
    }
    else
    {
        if (r_stdout[0] == 1)
        {
            printf("Invalid command: no output allowed for %s as redirected by pipe\n", command);
            return -1;
        }
        else
        {
            out = pipe_ends[1];
        }
    }
    if (last)
    {
        if (in != 0)
        {
            dup2(in, 0);
            close(in);
        }
        if (out != 1)
        {
            dup2(out, 1);
            close(out);
        }
        execute();
    }
    else
    {
        runInChildProcess(in, out);
        close(pipe_ends[1]);
    }
    return 0;
}

void parse_and_execute(char *input)
{
    // count the number of pipes
    int number_of_pipes = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '|')
            number_of_pipes++;
    }

    char *temp, *r = input;
    int total = 0;
    r_stdout_allowed = 0;
    r_stdin_allowed = 1;
    while (total < number_of_pipes)
    {
        temp = strtok_r(r, "|", &r);
        if (codeRunner(temp, 0) == -1)
        {
            close(pipe_ends[0]);
            close(pipe_ends[1]);
            dup2(original_stdin, 0);
            dup2(original_stdout, 1);
            return;
        }
        total++;
        r_stdin_allowed = 0;
    }
    temp = strtok_r(r, "|", &r);
    r_stdout_allowed = 1;
    if (number_of_pipes == 0)
        r_stdin_allowed = 1;
    else
    {
        r_stdin_allowed = 0;
    }
    codeRunner(temp, 1);
    // run each command
    if (number_of_pipes)
    {
        close(pipe_ends[0]);
        close(pipe_ends[1]);
    }
    dup2(original_stdin, 0);
    dup2(original_stdout, 1);
    return;
}