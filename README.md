# My shell - Pshell

Developed from scratch for CSE assignment

## To compile and execute:
```
    make
    ./Pshell
```


## Built-in Commands

All builtins are intended to work just as you may expect

```
ls, ll, la, cd, history, echo, pwd, clear, pinfo, setenv, unsetenv, jobs, kjob, fg, bg, overkill, quit, cronjob, nightswatch
```
Futher details on functionality can be read from assignment descriptions.

## Features
+ Input-output redirection with `|`, `>`, `<`, `>>` is supported 
+ commands like rm, man (or any non-builtins) can be executed as external processes
+ appending `&` at the final position will run the exerternal process as a backgound process
+ Each ;-separated command is executed separately
+ most builtins verify whether correct number of arguments and flags are provided
+ ls (ll, la) allows multiple arguments, and accordingly lists content of all the directories provided
+ *CTRL-Z* will stop a foreground process and push it to backgound
+ *CTRL-C* will only kill the a foreground process (if exists)
+ multiple up-arrows (`^[[A`) can be used to execute a previously executed command.

## General Pipeline

+ **shell.c** contains the main funtion with initializer like defining *~, user* etc.
+ It uses **takingInput.c** to *take_input* and calls the *parse_and_execute* from **redirection.c**
+ **redirection.c** breaks the commands separated by *pipe* & checks for *input-output redirection* to accordingly set the *stdin* and *stdout* of each command
+ It then uses *parseInput* function to break down command into flags, arguments etc
+ **driver.c** checks if the command is one of the *builtins* 
+ The **builtins.c, jobs.c, history.c, ls.c, nightswatch.c** (which also maintains linked list to track bg processes) define the various *builtin functions*
+ If the command isn't a builtin, the **shell.c** checks for *'&'* and accordingly runs the command as *background / foreground*
+ **externalProcessess.c** defines how to run the external processes, with interrupt handler to mark background process end
+ The **tilde.c** provides basic functions to convert between ~ and absolute value in a path (~ is set to the executation path of Pshell)
+ **makefile** compiles everything & produces the executable: **Pshell**