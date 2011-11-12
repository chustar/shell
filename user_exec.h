#ifndef USER_EXEC_H
#define USER_EXEC_H

#include <vector>
#include <string>
using namespace std;

int user_exec(vector<string>, vector<char>);
bool token_exec(vector<string>::iterator &, vector<char>::iterator &);
void fork_exec_bg(string);
bool resolve_exec();

#endif
