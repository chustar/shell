#ifndef LAUNCH_PROCESS_H
#define LAUNCH_PROCESS_H
#include <string.h>
#include <iostream>
#include <termios.h>

extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;

void init_shell(); //initialized the shell with the correct handling

void launch_process(std::string cmd); //launch a group of process

#endif
