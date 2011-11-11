/*
	Pie Shell [It's easy as Pie!]
	@Author: Pie Shell Team (Minh, Chuma, Abraham)
	Spring 2011
	bg.c Initial Prototyping of session, groups, tcgetpgrp, tcsetpgrp, tcgetsid
	group leader: Each process group can have a process group leader. The leader is identified by its process group ID being equal to its process ID. -- Advance Programming in UNIX


*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>

#define EXIT 0
#define QUIT "quit\n"
//#define DEBUG 1 
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

/* Make sure the shell is running interactively as the foreground job
	before proceeding*/

void init_shell() {
	/* see if we are running interactively. */
	shell_terminal = STDIN_FILENO;
	shell_is_interactive = isatty(shell_terminal);

	if(shell_is_interactive) {
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

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground) {
	pid_t pid;

	if(shell_is_interactive) {
		pid = getpid();
		
	}
}

int main() {
	pid_t pid,ppid,gpid,tpgid,sess;		//holds pid number
	pid_t call_id;
	pid_t history[10];
	int status;
	char input[4000];	//gets line input
	char *cmdArg[100];	//get argument to pass to execvp
	int i,j;		//for loop counters
	char *token;		//use to hold tokens
	char *bg;		//use to point to background if any
	

	/*
	Clear Screen for the Pie Shell
	*/
	pid = fork();
        if(pid < 0)
                printf("Fork Error!\n");
        else if(pid == 0) {
                execlp("clear",NULL,NULL);
                exit(0);
        }
        wait(&status);
	
/*	
	shell_terminal = STDIN_FILENO;	
        shell_is_interactive = isatty(shell_terminal);
	if(shell_is_interactive) {
		printf("%d %d\n", shell_terminal, shell_is_interactive);
	}
*/	
	init_shell();
	printf("It's Easy as Pie!\n");
        printf("Pie > ");
	

	fgets(input, sizeof input, stdin);
	while(strcmp(input, QUIT) != EXIT) {
#ifdef DEBUG
		printf("%s\n",input);
#endif
		bg = strchr(input,'&');

		token = strtok(input, " \n");
		i = 0;
		cmdArg[i] = token;
	

		while(token != NULL) {
			token = strtok(NULL, " \n");
			
#ifdef DEBUG
			if(token != NULL) 
				printf("%s\n", token);	
#endif		
			//do not add the & sign to run
			if(token != NULL && strcmp(token,"&") != 0)
				cmdArg[++i] = token;
		}
		cmdArg[++i] = NULL;
	
			
		pid = fork();
	
		//check for background command if there is then set foreground	
		/*if(pid > 0) {
			if(bg != NULL && strcmp(bg,"&") == 0) {
				history[1] = pid;
				tcsetpgrp(STDIN_FILENO, getpid());
			}else if(strcmp(input, "%1") == 0) {
				tcsetpgrp(STDIN_FILENO, history[1]);
			}
		}
		setpgid(0, 0);*/
#ifdef DEBUG	
		call_id = getpid();
		ppid = getppid();
		gpid = getpgrp();
		sess = getsid(pid);
		printf("pid ppid pgid sess\n");
		printf("%d %d %d %d\n",call_id,ppid,gpid,sess);	
#endif	
		if(pid < 0)
			printf("Fork Error!\n");
		else if(pid == 0) {
        
			execvp(cmdArg[0], cmdArg);
			exit(0); //this exit actually doesn't matter because the code is replaced when we did execvp
		} else {
			
		//	if(bg != NULL && strcmp(bg,"&") == 0) 
		//		waitpid(pid, &status, WNOHANG);
	//		else
				wait(&status);
			printf("Pie > ");
			fgets(input, sizeof input, stdin);
		}
	}

	return 0;	
}
