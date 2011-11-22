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
#include "var.h"

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
    bool fg = true; //holds flag if last process is in fg
    string state;
    G_BG_FLAG = false; //set global flag to false

    vector<string>::iterator myIter;
    for(myIter = cmd.begin(); myIter < cmd.end(); ++myIter) {
        if(*myIter == "&") //see if this is a background process
            G_BG_FLAG = true;
    }

    check_bg_status();
    store_history(cmd);
    if(cmd.size() != 0 && cmd[0] != "") {
        for(cmdIter = cmd.begin(), typeIter = types.begin(); cmdIter < cmd.end(); ++cmdIter, ++typeIter) {
            if(*typeIter == 'T') {
                token_exec(cmdIter, typeIter, cmd, fg);
            } else if(*typeIter == 'C') {
                stream_file[0] = "";
                stream_file[1] = "";
                stream_file[2] = "";
                fg = stream_exec(cmdIter, cmd);
            }
        }
        if(fg) {
            exit_pid = waitpid(child, &last_res, 0); //blocking wait for fg
        } else {
            exit_pid = waitpid(child, &last_res, WUNTRACED | WCONTINUED | WNOHANG);
            bg_status.push_back(last_res);
            state = get_state(last_res);
            cout<< exit_pid << " " << last_res << " " << state << " " << endl;
            //take care of condition when we access waitpid twice
            while(bg_status.size() > bg_process.size()) {
                bg_status.pop_back();
            }
        }

        return WEXITSTATUS(last_res);
    }
}

bool token_exec(vector<string>::iterator &cmdIter, vector<char>::iterator &typeIter, vector<string> &cmdV, bool fg) {
    int res = 0;
    string state;
    if(*cmdIter == "!") {                               //handle NOT condition
        neg = true;
    } else if(*cmdIter == "&&") {                       //handle AND condition
        cmdIter++;                                      //look ahead to the next command
        typeIter++;                                     //look ahead to the next typeIter
        if(fg) {
            waitpid(child, &res, 0);                        //check the status of the last run command
        } else {
            //check the status of the last run command
            exit_pid = waitpid(child, &res, WUNTRACED);
            bg_status.push_back(res);
            state = get_state(res);
            //	cout<< exit_pid << " " << res << " " << state << " " << endl;
        }
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
        } else if( res != 256 && res > 0 ) {
            //256 is the retrun number for exit(1) from ubuntu
            //we need to pause the rest of executioni
            string scmd;
            string stype;
            bg_cmd_process[bg_cmd_process.size()-1] += " && ";
            bg_dataV.back().compound = true;
            while(cmdIter < cmdV.end()) {
                bg_cmd_process[bg_cmd_process.size()-1] += *cmdIter;
                bg_cmd_process[bg_cmd_process.size()-1] += " ";

                scmd = *cmdIter;
                stype = *typeIter;
                bg_dataV.back().cmd.push_back(*cmdIter);
                bg_dataV.back().type.push_back(*typeIter);
                cmdIter++;
                typeIter++;
            }
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
        if(fg) {
            exit_pid = waitpid(child, &res, 0);
        } else {
            exit_pid = waitpid(child, &res, WUNTRACED);
            bg_status.push_back(res);
            state = get_state(res);
            //cout<< exit_pid << " " << res << " " << state << " " << endl;
        }
        if(neg) {
            if (res > 0) res = 0;
            else if (res == 0) res = 1;
        } //256 is the number returned on the ubuntu machine with exit(1)
        if (res != 0 & res == 256) { //if command is invalid run second command which we set with exit
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

bool stream_exec(vector<string>::iterator &cmdIter, vector<string> &vec) {
    string cmd = *cmdIter;
    string pos = "";
    int index = -1; //holds index of background process to bring to fg
    bool append = false;
    bool foreground = !G_BG_FLAG;
    bool wake_bg = false;
    int status;
    while (cmdIter < vec.end() && *cmdIter != "&&" && *cmdIter != "||" && *cmdIter != "|") {
        cout << *cmdIter << endl;
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
            //} else if(*cmdIter == "&") {
    } else if(G_BG_FLAG) {
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
    } else if(*cmdIter == "jobs") {
        display_jobs();
        return false; //only display background jobs
    } else if (*cmdIter == "history") {
        display_history();
        return false; //do not wait on these commands
    }
    cmdIter++;
    }
    if(cmdIter < vec.end() && *cmdIter == "|") {
        cmdIter++;
        fork_exec_pipe(cmd, foreground, append, cmdIter, vec);
    } else {
        cmdIter--;

        if(wake_bg) { //if it's moving a bg process to fg
            launch_foreground(index);
            return false;	//make sure we're not stuck in foreground
        } else {
            fork_exec_bg(cmd, foreground,append);
        }
    }
    return foreground;
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
    string state;

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

        /* Set the handling for job control signals back to the default.
           signal (SIGINT, SIG_DFL);
           signal (SIGQUIT, SIG_DFL);
           signal (SIGTSTP, SIG_DFL);
           signal (SIGTTIN, SIG_DFL);
           signal (SIGTTOU, SIG_DFL);
           signal (SIGCHLD, SIG_DFL); */

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
            cout << "OPEN OUT" << endl;
            close(out_fd[0]);
            dup2(out_fd[1], 1);
        }
        if (stream_file[STREAM_IN] != "") {
            cout << "OPEN IN" << endl;
            close(in_fd[1]);
            dup2(in_fd[0], 0);
        }
        if (stream_file[ERR_OUT] != "") {
            close(err_fd[0]);
            dup2(err_fd[1], 2);
        }
        execvp(cmdArg[0], cmdArg);
        exit(1); //will exit if it's an invalid command
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

        if (stream_file[STREAM_IN] != "") {
            waitpid(in_pid, &res, 0);
        }
        if (stream_file[STREAM_OUT] != "") {
            waitpid(out_pid, &res, 0);
        }
        if (stream_file[ERR_OUT] != "") {
            waitpid(err_pid, &res, 0);
        }

        if(!foreground) {
            setpgid(child,child);
            bg_process.push_back(child);
            bg_cmd_process.push_back(cmd.c_str());
            bg_dataV.push_back(temp);	//no compound cmd default
            bg_dataV.back().compound = false;
            printf("[%d] %d %s\n", (int)bg_process.size(), (int)child, cmd.c_str());
            /*exit_pid = waitpid(child, &status, WUNTRACED | WCONTINUED);
              bg_status.push_back(status);
              state = get_state(status);
              cout<< exit_pid << " " << status << " " << state << " " << endl;
              */
        } else {
                   		exit_pid = waitpid(child, &last_res, 0); //blocking wait for fg
                  //		cout<< exit_pid << "<-id  " << last_res << "<-status " << endl;
        }
              //	exit_pid = waitpid(WAIT_ANY, &status, WNOHANG);
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
        cout << "EXITING OUT" << endl;
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
        cout << "EXITING IN" << endl;
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
