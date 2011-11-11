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
		}
	}
	waitpid(child, &last_res, 0);
	return WEXITSTATUS(last_res);
}

bool token_exec(vector<string>::iterator &cmdIter, vector<char>::iterator &typeIter) {
	int res = 0;
	//set the negation if the command is !
    if(*cmdIter == "!") {
		cout << "Executing: !" << endl;
		neg = true;
    //handle AND condition.
	} else if(*cmdIter == "&&") {
		cout << "Executing: &&" << endl;
        cmdIter++;                                      //look ahead to the next command
		typeIter++;                                     //look ahead to the next type
        waitpid(child, &res, 0);                        //check the status of the last run command
		if(neg) {                                       //flip the response of the last command if negate was set.
			if (res > 0) res = 0;
			else if (res == 0) res = 1;
            neg = false;
		}
		if (res == 0) {                                 //if the last guy worked, execute it and leave it in the background
			cout << "Executing: &&: Next" << endl;
			if (*cmdIter == "!") {                      //handle a not condition before the next line
				cmdIter++;
				typeIter++;
                neg = true;
            }
            fork_exec_bg(*cmdIter);
		} else {                                        //if the command before and "failed" then ignore the next command (including its associated !
			if (*cmdIter == "!") {
				cmdIter++;
				typeIter++;
			}
			cmdIter++;
			typeIter++;
		}
	} else if(*cmdIter == "||"){
		cout << "Executing: ||" << endl;
		cmdIter++;
		typeIter++;
        waitpid(child, &res, 0);
        cout << res << endl;
		if(neg) {
			if (res > 0) res = 0;
			else if (res == 0) res = 1;
		}
		if (res != 0) {
			cout << "Executing: ||: Next" << endl;
			if (*cmdIter == "!") {
				cmdIter++;
				typeIter++;
                neg = true;
			}
			fork_exec_bg(*cmdIter);
		} else {
			if (*cmdIter == "!") {
				cmdIter++;
				typeIter++;
			}
			cmdIter++;
			typeIter++;
		}
		neg = false;
	} else if(*cmdIter == "|") {

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
