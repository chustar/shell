#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include "launch_process.h"
using namespace std;

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

vector<pid_t> bg_process;	//holds the pid_t of the bg process
vector<string> bg_cmd_process;	//holds the name of the cmd in parallel with bg_process
pid_t exit_pid;
vector<int> bg_status;

vector<string> history;
vector<int> history_status;
vector<BGDATA> bg_dataV;
BGDATA temp;

bool G_BG_FLAG = false;	//global background flag assumption: if there's one & then all is &

void init_shell() {

       /* see if we are running interactively. */
        shell_terminal = STDIN_FILENO;
        shell_is_interactive = isatty(shell_terminal);

        if(shell_is_interactive) {
                printf("interactive shell!\n");
            /*Loop until we are in the foreground*/
                while(tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
                        kill(-shell_pgid, SIGTTIN);

                /*Ignore interactive and job-control signals. */
     /*           signal(SIGINT, SIG_IGN);
                signal(SIGQUIT, SIG_IGN);
                signal(SIGTSTP, SIG_IGN);
                signal(SIGTTIN, SIG_IGN);
                signal(SIGTTOU, SIG_IGN);
                signal(SIGCHLD, SIG_IGN);
*/
               /* Put ourselves in our own process group. */
                shell_pgid = getpid();
                if(setpgid(shell_pgid, shell_pgid) < 0) {
                        perror("Couldnt put the shell in its own process group");
                        exit(1);
                }

                /* Grab control of the terminal. */
                tcsetpgrp(shell_terminal, shell_pgid);

                /* Save default terminal attributes for shell. */
                tcgetattr(shell_terminal, &shell_tmodes);
        }
}


void launch_process(std::string cmd, bool foreground) {
        pid_t process;
        int status;

        process = fork();
        if(process == 0) {
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
  
		setpgid(process,process); //set child process in it's own group       
		/* Set the handling for job control signals back to the default.  */
		signal (SIGINT, SIG_DFL);
		signal (SIGQUIT, SIG_DFL);
		signal (SIGTSTP, SIG_DFL);
		signal (SIGTTIN, SIG_DFL);
		signal (SIGTTOU, SIG_DFL);
		signal (SIGCHLD, SIG_DFL);

        	execvp (cmdArg[0], cmdArg);

        } else if( process < 0) {
                perror("fork error");
        } else {
                setpgid(process,process); //set child process in it's own group 
                waitpid(process, &status, WNOHANG);
        }


}

void launch_foreground(int index) {
	string cmd;
	bool c = false;
	vector<string> cmdV;
	vector<char> tV;	
	int status;
	pid_t proc;
                                
     	signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);



//need to handle bg process that exit already and should add this part into a function
        if(!bg_process.empty()) {
		if(index < 0) { //if less then zero it's fg command
			proc = bg_process.back();
			cmd = bg_cmd_process.back();
			tcsetpgrp(STDIN_FILENO, proc);
                	if (kill (- proc, SIGCONT) < 0)
                        	perror ("kill (SIGCONT)");
                	waitpid(proc, &status, 0);
                	cout << "[" << bg_process.size() << "] " <<  bg_process.back() << " " << bg_cmd_process.back() << endl; 	
			bg_process.pop_back();	//pop the id
			bg_cmd_process.pop_back(); //pop the cmd
                 
			/* Put the shell back in the foreground.  */
            tcsetpgrp (STDIN_FILENO, shell_pgid);
			

	signal (SIGINT, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
	signal (SIGTSTP, SIG_DFL);
	signal (SIGTTIN, SIG_DFL);
	signal (SIGTTOU, SIG_DFL);
	signal (SIGCHLD, SIG_DFL);
			for(int i = 0 ; i < (bg_dataV.back().cmd.size()); i++) {
				cmdV.push_back((bg_dataV.back().cmd[i]));
				tV.push_back((bg_dataV.back().type[i]));
			}
			c = bg_dataV.back().compound; 
			bg_dataV.pop_back();
			//check to see if we need a compound action	
			if(c) 
				user_exec(cmdV,tV );	
        		
		} else { //indexing into background process
			//check if index is out of range
			if(index < 0 || index > (int)bg_process.size())
				return;
			
			proc = bg_process[index-1];
			cmd = bg_cmd_process[index-1];
			tcsetpgrp(STDIN_FILENO, proc);
                	if (kill (- proc, SIGCONT) < 0)
                        	perror ("kill (SIGCONT)");
                	waitpid(proc, &status, 0);
                	bg_process.erase(bg_process.begin()+(index-1));
			bg_cmd_process.erase(bg_cmd_process.begin()+(index-1)); //removed hte cmd vector as well
                 
			bg_dataV.erase(bg_dataV.begin()+(index-1)); // dataV
			/* Put the shell back in the foreground.  */
                	tcsetpgrp (STDIN_FILENO, shell_pgid);
			
		}
	}


	signal (SIGINT, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
	signal (SIGTSTP, SIG_DFL);
	signal (SIGTTIN, SIG_DFL);
	signal (SIGTTOU, SIG_DFL);
	signal (SIGCHLD, SIG_DFL);

}


void display_jobs() {

	string state;

	for(int i = 0 ; i < bg_process.size(); i++) {
		state = get_state(bg_status[i]);
		cout << "[" << i+1 << "] " << state << " " << bg_process[i] << " " << bg_cmd_process[i] << endl;

		if(bg_dataV[i].compound) {
			for(int j = 0; j < bg_dataV[i].cmd.size(); j++) {
				cout<< "cmd: " << bg_dataV[i].cmd[j] << " type: " << bg_dataV[i].type[j] << endl;
			}
		
		}
	}

}

void check_bg_status() {
	int status;
	string state;
	for(int i = 0; i < bg_process.size(); i++) {
	        exit_pid = waitpid(bg_process[i], &status, WUNTRACED | WCONTINUED | WNOHANG);
               	//if less then zero, process error meaning it doesn't exist 
		//so kill process 
		if(exit_pid < 0 ) {
			state = get_state(status); //print exit status when we remove process
			//cout<<"["<< (i+1) << "] " << state << " " << bg_process[i] << " "<< bg_cmd_process[i] << endl;
                	bg_process.erase(bg_process.begin()+i);
                	bg_cmd_process.erase(bg_cmd_process.begin()+i);
                	bg_status.erase(bg_status.begin()+i);
		} else if (exit_pid > 0) { //process changed state so updated
			bg_status[i] = status;	
			state = get_state(status);
			//cout<<"["<< (i+1) << "] " << state << " " << bg_process[i] << " " << bg_cmd_process[i] << endl;
		} else { //process did not change state
			state = get_state(status);
//			cout<< exit_pid << " " << state << " " << endl;
		}
	}
}

string get_state(int status) {

                if( WIFEXITED(status) ) { 
                	return "T";
		}   
                if (WIFSIGNALED(status) ) { 
                	return "T+";
		}   
                if (WIFSTOPPED(status)) {
                	return "S";
		}   
                if (WIFCONTINUED(status) ) { 
                	return "C";
		}   

}

void store_history(vector<string> cmd) {
	string c;
	vector<string>::iterator cmdIter;
	
	for(cmdIter = cmd.begin(); cmdIter < cmd.end(); ++cmdIter) {
		c += *cmdIter;
		c += " ";
	}
	history.push_back(c);
}

void display_history() {

	vector<string>::iterator historyIter;
	int count = 0;
	for(historyIter = history.begin(); historyIter < history.end(); ++historyIter) {
		cout<< count << ": " << *historyIter << endl;
		count++;
	}
}

