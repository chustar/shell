#ifndef LAUNCH_PROCESS_H
#define LAUNCH_PROCESS_H
#include <string.h>
#include <iostream>
#include <termios.h>
#include <vector>
using namespace std;
extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;

extern vector<pid_t> bg_process;

void init_shell(); //initialized the shell with the correct handling

void launch_process(std::string cmd); //launch a group of process

void launch_foreground(int);

#endif
