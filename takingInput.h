#ifndef TAKINGINPUT_H 
#define TAKINGINPUT_H

int takeInput(char *buf, int arrC[4]);
int parseInput(char *input, char *command, char *flag, int *noOfArg, char **arguments, char *r_stdin, char *r_stdout);

#endif