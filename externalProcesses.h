#ifndef EXTERNALPROCESSES_H 
#define EXTERNALPROCESSES_H

void handler(int sig);
void background_2(char *command, char *flag, int noOfArg, char *arguments[]);
void foreground(char *command, char *flag, int noOfArg, char *arguments[]);

#endif