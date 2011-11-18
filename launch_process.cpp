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

vector<pid_t> bg_process;

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
                signal(SIGINT, SIG_IGN);
                signal(SIGQUIT, SIG_IGN);
                signal(SIGTSTP, SIG_IGN);
                signal(SIGTTIN, SIG_IGN);
                signal(SIGTTOU, SIG_IGN);
                signal(SIGCHLD, SIG_IGN);

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

	int status;
	pid_t proc;
//need to handle bg process that exit already and should add this part into a function
        if(!bg_process.empty()) {
		if(index < 0) { //if less then zero it's fg command
			proc = bg_process.back();
			tcsetpgrp(STDIN_FILENO, proc);
                	if (kill (- proc, SIGCONT) < 0)
                        	perror ("kill (SIGCONT)");
                	waitpid(proc, &status, 0);
                	bg_process.pop_back();

                 	/* Put the shell back in the foreground.  */
                	tcsetpgrp (STDIN_FILENO, shell_pgid);
        	} else { //indexing into background process
			//check if index is out of range
			if(index < 0 || index > (int)bg_process.capacity())
				return;
			
			proc = bg_process[index-1];
			tcsetpgrp(STDIN_FILENO, proc);
                	if (kill (- proc, SIGCONT) < 0)
                        	perror ("kill (SIGCONT)");
                	waitpid(proc, &status, 0);
                	bg_process.erase(bg_process.begin()+(index-1));

                 	/* Put the shell back in the foreground.  */
                	tcsetpgrp (STDIN_FILENO, shell_pgid);
			
		}
	}

}




