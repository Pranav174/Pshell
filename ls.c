
#include <unistd.h>
#include <stdio.h>
#include <sys/dir.h>
#include <string.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

void details(char *directory, char *filename)
{
    char temp[500];
    strcpy(temp, directory);
    strcat(temp, "/");
    strcat(temp, filename);
    // printf("stat of: %s\n", temp);
    struct stat statbuf;
    if (stat(temp, &statbuf) == -1)
    {
        perror("");
        return;
    }
    if (S_ISDIR(statbuf.st_mode))
        printf("d");
    else
        printf("-");
    int off = 1 << 8;
    while (off)
    {
        if (off & statbuf.st_mode)
            printf("r");
        else
            printf("-");
        off >>= 1;
        if (off & statbuf.st_mode)
            printf("w");
        else
            printf("-");
        off >>= 1;
        if (off & statbuf.st_mode)
            printf("x");
        else
            printf("-");
        off >>= 1;
    }
    printf(" ");
    printf("%ld ", statbuf.st_nlink);
    printf("%s ", getpwuid(statbuf.st_uid)->pw_name);
    printf("%s ", getgrgid(statbuf.st_gid)->gr_name);
    printf("%010ld ", statbuf.st_size);
    struct tm *my_tm = localtime(&statbuf.st_mtime);
    char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    printf("%s ", month[my_tm->tm_mon]);
    printf("%d ", my_tm->tm_mday);
    printf("%02d:%02d ", my_tm->tm_hour, my_tm->tm_min);
}

void ls(int noOfDirectories, char **directories, int l_flag, int a_flag)
{
    // allows ls to be performed on multiple directories at once
    if (noOfDirectories == 0)
    {
        noOfDirectories++;
        char temp[300];
        getcwd(temp, sizeof(temp));
        directories[0] = temp;
    }
    for (int i = 0; i < noOfDirectories; i++)
    {
        if (noOfDirectories > 1)
        {
            if (i >= 1)
            {
                printf("\n");
            }
            printf("\033[1;33m%s:\n\033[0m", directories[i]);
        }
        DIR *dirptr = opendir(directories[i]);
        if (dirptr == NULL)
        {
            perror("");
            continue;
        }
        struct dirent *entry = readdir(dirptr);
        while (entry != NULL)
        {
            if (a_flag != 0 || entry->d_name[0] != '.')
            {
                if (l_flag)
                {
                    details(directories[i], entry->d_name);
                }
                printf("%s\n", entry->d_name);
            }
            entry = readdir(dirptr);
        }
    }
}
void parse_ls(char *flag, int noOfArg, char **arguments)
{
    int l_flag = 0, a_flag = 0;
    for (int i = 1; i <= flag[0]; i++)
    {
        if (flag[i] == 'l')
        {
            l_flag = 1;
        }
        else if (flag[i] == 'a')
        {
            a_flag = 1;
        }
    }
    ls(noOfArg, arguments, l_flag, a_flag);
}