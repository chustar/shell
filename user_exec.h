#ifndef USER_EXEC_H
#define USER_EXEC_H

#include <vector>
#include <string>
using namespace std;

int user_exec(vector<string>, vector<char>);
bool token_exec(vector<string>::iterator &, vector<char>::iterator &, vector<string>);
void fork_exec_bg(string);
void stream_exec(vector<string>::iterator &, vector<string>);
void resolve_exec();
void fork_out_proc();
void fork_in_proc();
void fork_err_proc();
#endif
