
#ifndef JOBS_H 
#define JOBS_H

int noOfJobs;
struct child
{
    int pid;
    char command[200];
    int stat;
    struct child * next;
};
typedef struct child *job;
int fg_pid;
job head;
void addJob(int pid, char *command);
void addJobToLast(int pid, char *command);
job deleteJobByPid(int pid);
// void printAllJobs();
void printJobs();
void kjob(char *flag, int noOfArg, char **arguments);
void bg(char *flag, int noOfArg, char **arguments);
void fg(char *flag, int noOfArg, char **arguments);
void overkill(char *flag, int noOfArg, char **arguments);

#endif