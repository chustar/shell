#ifndef USER_EXEC_H
#define USER_EXEC_H

#include <vector>
#include <string>
#include <sys/wait.h>
using namespace std;

int user_exec(vector<string>, vector<char>);
bool token_exec(vector<string>::iterator &, vector<char>::iterator &, vector<string>&);
void fork_exec_bg(string);
void stream_exec(vector<string>::iterator &, vector<string> &);
void resolve_exec();
pid_t fork_out_proc();
pid_t fork_in_proc();
pid_t fork_err_proc();
#endif
