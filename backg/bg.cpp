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
	
		pro.argv = cmdArg;
		jo.first_process = &pro;
		
		launch_job(&jo, 0);	
        	printf("Pie > ");
		fgets(input, sizeof input, stdin);
	}

	return 0;	
}
