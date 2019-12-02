#include "shell.h"
#include <string.h>

void switchToTilde(char *path)
{
    int x = strlen(home);
    if (memcmp(home, path, x) == 0)
    {
        int y = strlen(path);
        path[0] = '~';
        for (int i = x; i <= y; i++)
        {
            path[i - x + 1] = path[i];
        }
    }
}

void replaceTilde(char *path)
{
    if (path[0] == '~')
    {
        char temp[200];
        strcpy(temp, path + 1);
        strcpy(path, home);
        strcat(path, temp);
    }
}