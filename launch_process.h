#ifndef LAUNCH_PROCESS_H
#define LAUNCH_PROCESS_H
#include <string.h>
#include <iostream>
void init_shell(); //initialized the shell with the correct handling

void launch_bg_process(std::string cmd); //launch a group of process

#endif
