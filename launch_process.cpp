#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "launch_process.h"


pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

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


void launch_bg_process(std::string cmd) {
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

