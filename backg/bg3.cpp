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

#include "util.h"

#define EXIT 0
#define QUIT "quit\n"
//#define DEBUG 1 
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

void fire_process(process *p, int foreground) {
	pid_t pid;
    	pid_t pgid; 
       if (shell_is_interactive)
         {
           /* Put the process into the process group and give the process group
              the terminal, if appropriate.
              This has to be done both by the shell and in the individual
              child processes because of potential race conditions.  */
           pid = getpid ();
           pgid = pid;
           setpgid (pid, pgid);
           if (foreground)
             tcsetpgrp (shell_terminal, pgid);
     
           /* Set the handling for job control signals back to the default.  */
           signal (SIGINT, SIG_DFL);
           signal (SIGQUIT, SIG_DFL);
           signal (SIGTSTP, SIG_DFL);
           signal (SIGTTIN, SIG_DFL);
           signal (SIGTTOU, SIG_DFL);
           signal (SIGCHLD, SIG_DFL);
         }

       /* Exec the new process.  Make sure we exit.  */
       execvp (p->argv[0], p->argv);
       perror ("execvp");
       exit (1);		
}

void powerup_process(job *j, int foreground) {
	pid_t pid;
	process *p;
	int status;
	p = j->first_process;
	pid = fork();
	if(pid == 0) {
		// Child Proces
		fire_process(p, foreground);
	} else if(pid < 0) {
		// Fork failed
		perror("fork");
		exit(1);
	} else {
		//Parent process
		p->pid = pid;
		if(shell_is_interactive) {
			j->pgid = pid;
			setpgid(pid, j->pgid);
		}
		if(foreground) {
			wait(&status);
		}else {
			waitpid(WAIT_ANY, &status, WNOHANG);
		}
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
		
	struct process pro;
	struct job jo;
	
	int backflag;
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
	
	init_shell();
	printf("It's Easy as Pie!\n");
        printf("Pie > ");

	fgets(input, sizeof input, stdin);
	while(strcmp(input, QUIT) != EXIT) {
#ifdef DEBUG
		printf("%s\n",input);
#endif
		bg = strchr(input,'&');
		backflag = 1;

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
			else if( token != NULL && strcmp(token, "&") == 0) {
				backflag = 0;
			}
		}
		cmdArg[++i] = NULL;
	
		pro.argv = cmdArg;
		jo.first_process = &pro;
		
		launch_job(&jo, backflag);	
        	printf("Pie > ");
		fgets(input, sizeof input, stdin);
	}

	return 0;	
}
