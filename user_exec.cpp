#include <iostream>
#include <vector>
#include "user_exec.h"

using namespace std;

int negate = false;
int last_res = 0;

int user_exec(vector<string> cmd, vector<char> types) {
	int child = 0;
	vector<string>::iterator cmdIter;
	vector<char>::iterator typeIter;
	for(cmdIter = cmd.begin(), typeIter = types.begin(); cmdIter < cmd.end(); ++cmdIter, ++typeIter) {
		if(*typeIter == 'T') {
			token_exec(cmdIter, typeIter);
		} else if(*typeIter == 'C') {
			child = fork_exec_bg(*cmdIter);
		} else {
			resolve_exec(child);
		}
	}
	resolve_exec(child);
}

bool token_exec(vector<string>::iterator cmdIter, vector<char>::iterator typeIter) {
	int res = 0;
	if(*cmdIter == "!") {
		cmdIter++;
		typeIter++;
		res = fork_exec_bg(*cmdIter);
		if (res > 0) last_res = 0;
		if (res == 0) last_res = 1;
	} else if(*cmdIter == "&&") {
		cmdIter++;
		typeIter++;
		if (last_res == 0) {
			if (*cmdIter == "!") {
				cmdIter++;
				typeIter++;
				res = fork_exec_bg(*cmdIter);
				if (res > 0) last_res = 0;
				if (res == 0) last_res = 1;
			} else {
				last_res = fork_exec_bg(*cmdIter);
			}
		}
	} else if(*cmdIter == "||") {

	}
}

int fork_exec_bg(string v) {

}

bool resolve_exec(int child) {
	if(child != 0) {
/*
		switch(exec_env) {
			case AND:

			break;
			case OR:

			break;
			case STREAM_IN:

			break;
			case STREAM_OUT:

			break;
			case STREAM_ERR:

			break;
			case PIPE:

			break;
			case BACKGROUND:

			break;
		}
*/
	}
}
