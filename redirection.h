#ifndef REDIRECTION_H 
#define REDIRECTION_H

int original_stdin, original_stdout;
char *arguments[100];
void parse_and_execute(char *input);

#endif