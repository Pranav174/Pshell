#ifndef BUILTINS_H 
#define BUILTINS_H


void cd(char *flag, int noOfArg, char *arguments);
void echo(char *flag, int noOfArg, char **arguments);
void pwd(char *flag, int noOfArg);
void pinfo(char *flag, int noOfArg, char **arguments);
void cronjob(char *flag, int noOfArg, char **arguments);
void b_setenv(char *flag, int noOfArg, char **arguments);
void b_unsetenv(char *flag, int noOfArg, char **arguments);
void clear();

#endif