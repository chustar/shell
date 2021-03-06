#ifndef LAUNCH_PROCESS_H
#define LAUNCH_PROCESS_H
#include <string.h>
#include <iostream>
#include <termios.h>
#include <vector>
#include "user_exec.h"

using namespace std;
extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;

//data structure in parallel, probably should've put them in a class/struct....
extern vector<pid_t> bg_process;	//use to hold pid of bg
extern vector<string> bg_cmd_process;	//used to hold command of pid
extern vector<int> bg_status;		//use to hold status of the pid

extern pid_t exit_pid;	//holds the id of exit process from waitpid

extern vector<string> history;
extern vector<int> history_status;

struct BGDATA {
	bool compound;		//true if bg_cmd_process is multiple instr
	vector<string> cmd;	//holds commands to be process through history
	vector<char> type;	//holds the types of the command
};

extern BGDATA temp;		//holds a template to pushback inot vector
extern vector<BGDATA> bg_dataV;

extern bool G_BG_FLAG;

void init_shell(); //initialized the shell with the correct handling

void launch_foreground(int); //bring process to the foreground

void display_jobs();	//display all the background process

void check_bg_status();	//check bg process and remove/update process bg

string get_state(int);	//takes status and returns the state of the status

void store_history(vector<string>);

void store_status_cmd(int);

void display_history();

void display_exit_status(int);

#endif
