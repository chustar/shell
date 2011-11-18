#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "user_exec.h"
#include "launch_process.h"

using namespace std;

pid_t child = 0;
int last_res = 0;
bool neg = false;
int out_fd[2];
int in_fd[2];
int err_fd[2];
string stream_file[3];

//vector<pid_t> bg_process;

int STREAM_OUT = 0;
int STREAM_IN = 1;
int ERR_OUT = 2;

int user_exec(vector<string> cmd, vector<char> types) {
	vector<string>::iterator cmdIter;
	vector<char>::iterator typeIter;
    if(cmd.size() != 0 && cmd[0] != "") {
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
        return WEXITSTATUS(last_res);
    }
}

bool token_exec(vector<string>::iterator &cmdIter, vector<char>::iterator &typeIter, vector<string> &cmdV) {
	int res = 0;
    if(*cmdIter == "!") {                               //handle NOT condition
		neg = true;
	} else if(*cmdIter == "&&") {                       //handle AND condition
        cmdIter++;                                      //look ahead to the next command
		typeIter++;                                     //look ahead to the next typeIter
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
	}
}

void stream_exec(vector<string>::iterator &cmdIter, vector<string> &vec) {
    string cmd = *cmdIter;
	string pos = "";
    	int index = -1; //holds index of background process to bring to fg
	bool append = false;
	bool foreground = true;
	bool wake_bg = false;
	int status;
	while (cmdIter < vec.end() && *cmdIter != "&&" && *cmdIter != "||" && *cmdIter != "|") {
        if(*cmdIter == "<") {
            cmdIter++;
            stream_file[STREAM_IN] = *cmdIter;
        } else if(*cmdIter == ">") {
            cmdIter++;
            stream_file[STREAM_OUT] = *cmdIter;
        } else if(*cmdIter == ">>") {
            cmdIter++;
            stream_file[STREAM_OUT] = *cmdIter;
            append = true;
        } else if(*cmdIter == "2>") {
            cmdIter++;
            stream_file[ERR_OUT] = *cmdIter;
        } else if(*cmdIter == "2>>") {
            cmdIter++;
            stream_file[ERR_OUT] = *cmdIter;
            append = true;
        } else if(*cmdIter == "&") {
			foreground = false;
	} else if(*cmdIter == "fg") {
			wake_bg = true;
	} else if((*cmdIter).compare(0,1,"%") == 0) {
	//	printf("woot: %s\n",(cmdIter));
		pos = *cmdIter;
		//remove the % from the string
		pos.replace(0,1,"");
		index = atoi(pos.c_str());
		cout << "woot: " << index << endl;
		wake_bg = true;
	}
       	cmdIter++;
   	}
    if(cmdIter < vec.end() && *cmdIter == "|") {
        cmdIter++;
        fork_exec_pipe(cmd, foreground, append, cmdIter, vec);
    } else {
        cmdIter--;
        if(wake_bg) {
            if(!bg_process.empty()) {
                tcsetpgrp(STDIN_FILENO, bg_process.back());
                if (kill (- bg_process.back(), SIGCONT) < 0)
                        perror ("kill (SIGCONT)");
                waitpid(bg_process.back(), &status, 0);
                bg_process.pop_back();

                 /* Put the shell back in the foreground.  */
                tcsetpgrp (STDIN_FILENO, shell_pgid);
            }
            launch_foreground(index);
        } else {
            fork_exec_bg(cmd, foreground,append);
        }
    }
}

void fork_exec_pipe(string cmd, bool foreground, bool append, vector<string>::iterator &cmdIter, vector<string> &vec) {
    int res;
    int pipe_fd[2];
    pipe(pipe_fd);
    pid_t src_pid;
    pid_t dest_pid;

    src_pid = fork();
    if(src_pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], 1);
        fork_exec_bg(cmd, foreground, append);
        exit(0);
    }

    dest_pid = fork();
    if(dest_pid == 0) {
        close(pipe_fd[1]);
        dup2(pipe_fd[0], 0);
        stream_exec(cmdIter, vec);
        exit(0);
    }

	while (cmdIter < vec.end() && *cmdIter != "&&" && *cmdIter != "||") {
        cmdIter++;
    }
    cmdIter--;

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(src_pid, &res, 0);
    waitpid(dest_pid, &res, 0);
}
//johnny one note
void fork_exec_bg(string cmd, bool foreground, bool append) {
	int status;

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
        pid_t childId;
        cmdArg[i] = token;

		/* Set the handling for job control signals back to the default.  */
		signal (SIGINT, SIG_DFL);
		signal (SIGQUIT, SIG_DFL);
		signal (SIGTSTP, SIG_DFL);
		signal (SIGTTIN, SIG_DFL);
		signal (SIGTTOU, SIG_DFL);
		signal (SIGCHLD, SIG_DFL);

		if(!foreground) {
			childId = getpid();
			setpgid(childId,childId); //set child process in it's own group
		}

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
            dup2(in_fd[0], 0);
        }
       if (stream_file[ERR_OUT] != "") {
            close(err_fd[0]);
            dup2(err_fd[1], 2);
        }

        execvp(cmdArg[0], cmdArg);
	} else {
        int res;
        pid_t out_pid;
        pid_t in_pid;
        pid_t err_pid;

        if (stream_file[STREAM_OUT] != "") {
            close(out_fd[1]);
            out_pid = fork_out_proc(append);
            close(out_fd[0]);
        }
        if (stream_file[STREAM_IN] != "") {
            close(in_fd[0]);
            in_pid = fork_in_proc();
            close(in_fd[1]);
        }
       if (stream_file[ERR_OUT] != "") {
            close(err_fd[1]);
            err_pid = fork_err_proc(append);
            close(err_fd[0]);
        }

        if (stream_file[STREAM_OUT] != "") {
            waitpid(out_pid, &res, 0);
        }
        if (stream_file[STREAM_IN] != "") {
            waitpid(in_pid, &res, 0);
        }
       if (stream_file[ERR_OUT] != "") {
            waitpid(err_pid, &res, 0);
        }

		if(!foreground) {
			setpgid(child,child);
			bg_process.push_back(child);
			printf("[%d] %s\n", (int)bg_process.capacity(), cmd.c_str());
			waitpid(child, &status, WNOHANG);
		} else {
        		waitpid(child, &last_res, 0);
		}
		waitpid(WAIT_ANY, &status, WNOHANG);
    }
}

pid_t fork_out_proc(bool append) {
    pid_t out_pid;
    if((out_pid = fork()) == 0) {
        long lSize = 10000;
        char * buffer;
        buffer = new char[lSize];
        close(out_fd[1]);
        int len = read(out_fd[0], buffer, lSize);
        string out(buffer);

        ofstream fout(stream_file[STREAM_OUT].c_str(), append ? ios_base::app : ios_base::trunc);
        fout << out;
        close(out_fd[0]);
        fout.close();
        delete [] buffer;
        exit(0);
    } else {
        return out_pid;
    }
}

pid_t fork_in_proc() {
    string line;
    pid_t in_pid;
    if((in_pid = fork()) == 0) {
        close(in_fd[0]);
        ifstream file(stream_file[STREAM_IN].c_str(), ios_base::in);
        if (file.bad()) {
            cerr << "File read error\n";
            return 1;
        }

        while(file.good()) {
            getline(file, line);
            if(file.good()) {
                write(in_fd[1], line.c_str(), line.length());
            }
        }
        file.close();
        close(in_fd[1]);
        exit(0);
    } else {
        return in_pid;
    }
}

pid_t fork_err_proc(bool append) {
    pid_t err_pid;
    if((err_pid = fork()) == 0) {
        long lSize = 10000;
        char * buffer;
        buffer = new char[lSize];
        close(err_fd[1]);
        int len = read(err_fd[0], buffer, lSize);
        string out(buffer);

        ofstream fout(stream_file[ERR_OUT].c_str(), append ? ios_base::app : ios_base::trunc);
        fout << out;

        fout.close();
        close(err_fd[0]);
        delete [] buffer;
        exit(0);
    } else {
        return err_pid;
    }
}
