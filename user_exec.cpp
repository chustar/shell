#include <iostream>
#include <vector>

using namespace std;

int user_exec(vector<string>, vector<char>);
bool token_exec(vector<string>::iterator &, vector<char>::iterator &);
void fork_exec_bg(string);
bool resolve_exec();

pid_t child = 0;
int last_res = 0;
bool neg = false;

int user_exec(vector<string> cmd, vector<char> types) {
	vector<string>::iterator cmdIter;
	vector<char>::iterator typeIter;
	for(cmdIter = cmd.begin(), typeIter = types.begin(); cmdIter < cmd.end(); ++cmdIter, ++typeIter) {
		if(*typeIter == 'T') {
			token_exec(cmdIter, typeIter);
		} else if(*typeIter == 'C') {
			fork_exec_bg(*cmdIter);
		} else {
			resolve_exec();
		}
	}
	resolve_exec();
}

bool token_exec(vector<string>::iterator &cmdIter, vector<char>::iterator &typeIter) {
	int res = 0;
	if(*cmdIter == "!") {
		cout << "Executing: !" << endl;
		neg = true;
	} else if(*cmdIter == "&&") {
		cout << "Executing: &&" << endl;
		cmdIter++;
		typeIter++;
		//check the currently backgrounded app
//		if(child != 0) {
			waitpid(child, &res, 0);
//			res = WEXITSTATUS(res);
//			last_res = res;
			cout << res << endl;
//			if (neg && !WIFEXITED(res)) last_res = 0;
//			if (neg && WIFEXITED(res)) last_res = 1;
//			neg = false;
//		}
		if ((neg && !WIFEXITED(res)) || WIFEXITED(res)) {
			cout << "Executing: &&: Next" << endl;
			if (*cmdIter == "!") {
				cmdIter++;
				typeIter++;
				fork_exec_bg(*cmdIter);
				if (res > 0) last_res = 0;
				if (res == 0) last_res = 1;
			} else {
				fork_exec_bg(*cmdIter);
			}
		}
		neg = false;
	} else if(*cmdIter == "||") {

	}
}

void fork_exec_bg(string cmd) {
	cout << "Executing: " << cmd << endl;
	child = fork();
	if(child == 0) {
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

bool resolve_exec() {
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
