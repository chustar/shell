#ifndef USER_EXEC_H
#define USER_EXEC_H

#include <vector>
#include <string>
#include <sys/wait.h>
using namespace std;

int user_exec(vector<string>, vector<char>);
bool token_exec(vector<string>::iterator &, vector<char>::iterator &, vector<string>&, bool);
void fork_exec_pipe(string, bool, bool, vector<string>::iterator &, vector<string> &);
void fork_exec_bg(string, bool, bool);
bool stream_exec(vector<string>::iterator &, vector<string> &);
pid_t fork_out_proc(bool);
pid_t fork_in_proc();
pid_t fork_err_proc(bool);
#endif
