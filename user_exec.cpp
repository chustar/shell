#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "user_exec.h"
#include "launch_process.h"

using namespace std;

pid_t child = 0;
int last_res = 0;
bool neg = false;
int out_fd[2];
int in_fd[2];
int err_fd[3];
string stream_file[3];

int STREAM_OUT = 0;
int STREAM_IN = 1;
int ERR_OUT = 2;

int user_exec(vector<string> cmd, vector<char> types) {
	vector<string>::iterator cmdIter;
	vector<char>::iterator typeIter;
    if(cmd.size() != 0) {
        for(cmdIter = cmd.begin(), typeIter = types.begin(); cmdIter < cmd.end(); ++cmdIter, ++typeIter) {
            if(*typeIter == 'T') {
                token_exec(cmdIter, typeIter, cmd);
            } else if(*typeIter == 'C') {
                stream_file[0] = "";
                stream_file[1] = "";
                stream_file[2] = "";
                stream_exec(cmdIter, cmd);
            }
        }
    //    resolve_exec();
        waitpid(child, &last_res, 0);
        return WEXITSTATUS(last_res);
    }
}

bool token_exec(vector<string>::iterator &cmdIter, vector<char>::iterator &typeIter, vector<string> &cmdV) {
	int res = 0;
	//set the negation if the command is !
    if(*cmdIter == "!") {
		neg = true;
    //handle AND condition.
	} else if(*cmdIter == "&&") {
        cmdIter++;                                      //look ahead to the next command
		typeIter++;                                     //look ahead to the next typeIter
//        resolve_exec();
        waitpid(child, &res, 0);                        //check the status of the last run command
		if(neg) {                                       //flip the response of the last command if negate was set.
			if (res > 0) res = 0;
			else if (res == 0) res = 1;
            neg = false;
		}
		if (res == 0) {                                 //if the last guy worked, execute it and leave it in the background
			if (*cmdIter == "!") {                      //handle a not condition before the next line
				cmdIter++;
				typeIter++;
                neg = true;
            }
            stream_exec(cmdIter, cmdV);
		} else {                                        //if the command before and "failed" then ignore the next command (including its associated !
			if (*cmdIter == "!") {
				cmdIter++;
				typeIter++;
			}
			cmdIter++;
			typeIter++;
		}
	} else if(*cmdIter == "||"){
		cmdIter++;
		typeIter++;
//        resolve_exec();
        waitpid(child, &res, 0);
		if(neg) {
			if (res > 0) res = 0;
			else if (res == 0) res = 1;
		}
		if (res != 0) {
			if (*cmdIter == "!") {
				cmdIter++;
				typeIter++;
                neg = true;
			}
			 stream_exec(cmdIter, cmdV);
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

//johnny one note
void fork_exec_bg(string cmd) {
    if (stream_file[STREAM_OUT] != "")
        pipe(out_fd);
    if (stream_file[STREAM_IN] != "")
        pipe(in_fd);
    if (stream_file[ERR_OUT] != "")
        pipe(err_fd);

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

        if (stream_file[STREAM_OUT] != "") {
            close(out_fd[0]);
            dup2(out_fd[1], 1);
        }
        if (stream_file[STREAM_IN] != "") {
            close(in_fd[1]);
            dup2(out_fd[0], 0);
        }
       if (stream_file[ERR_OUT] != "") {
            close(err_fd[0]);
            close(err_fd[1]);
            dup2(out_fd[2], 2);
        }

        execvp(cmdArg[0], cmdArg);
	} else {
        if (stream_file[STREAM_OUT] != "") {
            close(out_fd[1]);
            fork_out_proc();
        }
        if (stream_file[STREAM_IN] != "") {
            close(in_fd[0]);
            fork_in_proc();
        }
       if (stream_file[ERR_OUT] != "") {
            close(err_fd[1]);
            close(err_fd[2]);
            fork_err_proc();
        }
    }
}

void fork_out_proc() {
    if(fork() == 0) {
        long lSize = 10000;
        char * buffer;
        buffer = new char[lSize];
        close(out_fd[1]);
        int len = read(out_fd[0], buffer, lSize);
        string out(buffer);

        ofstream fout(stream_file[STREAM_OUT].c_str(), ios_base::trunc);
        fout << out;

        fout.close();
        delete [] buffer;
    }
}

void fork_in_proc() {
    /*
    if(fork() == 0) {
        long lSize = 10000;
        char * buffer;
        buffer = new char[lSize];
        close(out_fd[0]);
        int len = read(out_fd[1], buffer, lSize);
        string out(buffer);

        ofstream fout(stream[STREAM_OUT], ios_base::trunc);
        fout << out;

        fout.close();
        delete [] buffer;
    }
    */
}

void fork_err_proc() {
    if(fork() == 0) {
        long lSize = 10000;
        char * buffer;
        buffer = new char[lSize];
        close(out_fd[1]);
        close(err_fd[2]);
        int len = read(err_fd[0], buffer, lSize);
        string out(buffer);

        ofstream fout(stream_file[ERR_OUT].c_str(), ios_base::trunc);
        fout << out;

        fout.close();
        delete [] buffer;
    }
}

void stream_exec(vector<string>::iterator &cmdIter, vector<string> &vec) {
    string cmd = *cmdIter;
    bool foreground = true;
    while (cmdIter < vec.end() && *cmdIter != "&&" && *cmdIter != "||") {
        if(*cmdIter == "<") {
            cmdIter++;
            stream_file[STREAM_IN] = *cmdIter;
        } else if(*cmdIter == ">" || *cmdIter == ">>") {
            cmdIter++;
            stream_file[STREAM_OUT] = *cmdIter;
        } else if(*cmdIter == "2>" || *cmdIter == "2>>") {
            cmdIter++;
            stream_file[ERR_OUT] = *cmdIter;
        }
	else if(*cmdIter == "&") {
		foreground = false;
	}
        cmdIter++;
    }
    cmdIter--;
    if(foreground)  
    	fork_exec_bg(cmd);
    else
	launch_bg_process(cmd);
}

void resolve_exec() {
    long lSize = 10000;
    char * buffer;
    buffer = new char[lSize];

    int len = read(out_fd[0], buffer, lSize);
    string out(buffer);
    cout << out;
    len = read(err_fd[0], buffer, lSize);
    out = string(buffer);
    cout << out;
    waitpid(child, &last_res, 0);
}
