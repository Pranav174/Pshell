#include <string.h>
#include <stdlib.h>
#include "tilde.h"
#include "shell.h"
#include <unistd.h>
#include <stdio.h>

int takeInput(char *buf, int arrC[4])
{
    for (int i = 0; i < 4; i++)
    {
        arrC[i] = 0;
    }
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    switchToTilde(cwd);
    printf("\033[1;32m%s@%s: \033[1;34m%s\033[1;32m> \033[0m", user, host, cwd);
    // printf("inpust success :%s\n", bbuf);
    // fgets(buf,1000, stdin);
    // scanf("%s", buf);
    int c, n = 0;
    while (c = getchar())
    {
        if (c == EOF || c == '\n')
        {
            if (c == EOF)
            {
                printf("\n");
            }
            buf[n] = '\0';
            break;
        }
        else if (c == '\033')
        {
            c = getchar();
            c = getchar();
            switch (c)
            {
            case 'A':
                arrC[0]++;
                break;
            case 'B':
                arrC[1]++;
                break;

            case 'C':
                arrC[2]++;
                break;

            case 'D':
                arrC[3]++;
                break;
            }
        }
        else
        {
            buf[n++] = c;
        }
    }
    // printf("input was :%s:\n", buf);
    // printf("n was :%d:\n", n);
    // for (int i = 0; i < 4; i++)
    // {
    //     printf("i: %d\n", arrC[i]);
    // }
    // printf("input success :%s\n", buf);
    return strlen(buf);
}

// int takeInput(char *buf)
// {
//     char cwd[1024];
//     getcwd(cwd, sizeof(cwd));
//     switchToTilde(cwd);
//     char promt[500];
//     sprintf(promt, "\033[1;32m%s@%s: \033[1;34m%s\033[1;32m> \033[0m", user, host, cwd);
//     // printf("input success :%s\n", buf);

//     char *bbuf = readline(promt);
//     // printf("inpust success :%s\n", bbuf);

//     if (bbuf==NULL){
//         bbuf = (char*) malloc(sizeof(char));
//         printf("\n");
//         bbuf[0]='\0';
//     }
//     // printf("inpvdsvust success :%s\n", bbuf);

//     strcpy(buf, bbuf);
//     free(bbuf);
//     // printf("input success :%s\n", buf);
//     return strlen(buf);
// }

int parseInput(char *ini_input, char *command, char *flag, int *noOfArg, char **arguments, char *r_stdin, char *r_stdout)
{
    r_stdin[0] = 0;
    r_stdout[0] = 0;
    r_stdout[1] = 0;
    int x = 0, cur = 0, skip = 1, quotes = 0;
    char input[500];
    while (ini_input[x] != '\0')
    {
        if (ini_input[x] == ' ' || ini_input[x] == '\t')
        {
            if (skip)
            {
                x++;
            }
            else
            {
                if (quotes)
                {
                    input[cur++] = ini_input[x++];
                }
                else
                {
                    // add ; as a delimiter between tokens while the spaces in quotes are maintained
                    input[cur++] = ';';
                    x++;
                    skip = 1;
                }
            }
        }
        else if (ini_input[x] == '"')
        {
            skip = 0;
            if (quotes)
            {
                x++;
                quotes = 0;
            }
            else
            {
                x++;
                quotes = 1;
            }
        }
        else if (ini_input[x] == '<' && quotes == 0)
        {
            input[cur++] = ';';
            input[cur++] = ini_input[x++];
            input[cur++] = ';';
            skip = 1;
        }
        else if (ini_input[x] == '>' && quotes == 0)
        {
            input[cur++] = ';';
            if (ini_input[x + 1] == '>')
            {
                input[cur++] = ini_input[x++];
                input[cur++] = ini_input[x++];
            }
            else
            {

                input[cur++] = ini_input[x++];
            }
            input[cur++] = ';';
            skip = 1;
        }
        else
        {
            input[cur++] = ini_input[x++];
            skip = 0;
        }
        // char x = input[cur];
        // input[cur] = '\0';
        // printf("after modification: %s\n", input);
        // input[cur]=x;
    }
    input[cur] = '\0';
    // printf("after modification: %s\n", input);
    if (quotes)
    {
        return 1;
    }
    flag[0] = 0;
    *noOfArg = 0;
    char *temp = strtok(input, ";");
    strcpy(command, temp);
    temp = strtok(NULL, ";");
    while (temp != NULL)
    {
        if (temp[0] == '\0')
        {
            continue;
        }
        if (temp[0] == '-')
        {
            for (int i = 1; temp[i] != '\0'; i++)
            {
                flag[++flag[0]] = temp[i];
            }
        }
        else if (strcmp(temp, "<") == 0)
        {
            r_stdin[0] = 1;
            temp = strtok(NULL, ";");
            if (temp == NULL)
            {
                return 2;
            }
            strcpy(r_stdin + 1, temp);
        }
        else if (strcmp(temp, ">") == 0)
        {
            r_stdout[0] = 1;
            r_stdout[1] = 0;
            temp = strtok(NULL, ";");
            if (temp == NULL)
            {
                return 2;
            }
            strcpy(r_stdout + 2, temp);
        }
        else if (strcmp(temp, ">>") == 0)
        {
            r_stdout[0] = 1;
            r_stdout[1] = 1;
            temp = strtok(NULL, ";");
            if (temp == NULL)
            {
                return 2;
            }
            strcpy(r_stdout + 2, temp);
        }
        else
        {
            strcpy(arguments[(*noOfArg)++], temp);
        }
        temp = strtok(NULL, ";");
    }
    return 0;
}