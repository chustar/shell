#include <iostream>
#include <vector>

using namespace std;

int user_exec(vector<string>, vector<char>);
bool token_exec(vector<string>::iterator &, vector<char>::iterator &);
int fork_exec_bg(string);
bool resolve_exec(int);

int child = 0;
int last_res = 0;

int user_exec(vector<string> cmd, vector<char> types) {
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

bool token_exec(vector<string>::iterator &cmdIter, vector<char>::iterator &typeIter) {
	int res = 0;
	if(*cmdIter == "!") {
		cmdIter++;
		typeIter++;
		res = fork_exec_bg(*cmdIter);
		if (res > 0) last_res = 0;
		if (res == 0) last_res = 1;
		cout << "Executing: !" << endl;
	} else if(*cmdIter == "&&") {
		cout << "Executing: &&" << endl;
		cmdIter++;
		typeIter++;
		if (last_res == 0) {
			cout << "Executing: &&: Next" << endl;
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

int fork_exec_bg(string cmd) {
	cout << "Executing: " << cmd << endl;
	child = fork();
	if(child > 0) {
		char *input = (char*)cmd.c_str();
		char *cmdArg[100];
		char *token = strtok(input, " \n");
		int i = 0;
		cmdArg[i] = token;

		//while this is not the last token
		while(token != NULL) {
			token = strtok(NULL, " \n");
			cmdArg[++i] = token;
		}
		cmdArg[++i] = NULL;
		
		execvp(cmdArg[0], cmdArg);
	}
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
