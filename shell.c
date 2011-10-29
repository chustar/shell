/*
	Pie Shell [It's easy as Pie!]
	@Author: Pie Shell Team (Minh, Chuma, Abraham)
	Spring 2011
	Shell1.c Initial Prototyping of execvp
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vars.c"

#define EXIT 0
#define QUIT "quit\n"
//#define DEBUG 0 

void main() {
	int pid;		//holds pid number
	char input[4000];	//gets line input
	char *cmdArg[100];	//get argument to pass to execvp
	int i,j;		//for loop counters
	char *token;		//use to hold tokens

	load_vars();

	/*
	Clear Screen for the Pie Shell
	*/
	pid = fork();
	if(pid < 0) {
		printf("Fork Error!\n");
	} else if(pid == 0) {
		execlp("clear", NULL, NULL);
		exit(0);
	}
	wait(pid);

	printf("It's Easy as Pie!\n");
	printf("Pie > ");
	fgets(input, sizeof input, stdin);

	while(strcmp(input, QUIT) != EXIT) {
#ifdef DEBUG
		printf("%s\n",input);
#endif
		token = strtok(input, " \n");
		i = 0;
		cmdArg[i] = token;
		while(token != NULL) {
			token = strtok(NULL, " \n");
			cmdArg[++i] = token;
		}
		cmdArg[++i] = NULL;
	
		pid = fork();
		if(pid < 0)
			printf("Fork Error!\n");
		else if(pid == 0) {
			execvp(cmdArg[0], cmdArg);
			exit(0);
		} else {
			wait(pid);
			printf("Pie > ");
			fgets(input, sizeof input, stdin);
		}
	
	}
	return;	
}
