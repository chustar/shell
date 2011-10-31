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
#include "var.c"

#define EXIT 0
#define QUIT "quit\n"
//#define DEBUG 0 

int main() {
	pid_t pid;		//holds pid number
	int status;
	char input[4000];	//gets line input
	char *cmdArg[100];	//get argument to pass to execvp
	int i,j;		//for loop counters
	char *token;		//use to hold tokens

	/*
	Load user specified variables into environment map
	*/
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
	wait(&status);

	//sigh...
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

		//while this is not the last token
		while(token != NULL) {
			token = strtok(NULL, " \n");
			cmdArg[++i] = token;
		}
		cmdArg[++i] = NULL;
	
		//fork the process here to run the new shell
		pid = fork();
		if(pid < 0)
			printf("Fork Error!\n");
		else if(pid == 0) {
			//maybe change to our own function called parse_exec to parse and execute?
			execvp(cmdArg[0], cmdArg);
			exit(0);
		} else {
			wait(&status);
			printf("Pie > ");
			fgets(input, sizeof input, stdin);
		}
	
	}
	return 0;	
}
