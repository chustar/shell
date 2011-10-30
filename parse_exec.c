#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "var.c"

int parse_exec(char **cmdArg) {
	//check if its a variable set, otherwise just execute it	
	int var_set = 1;
	if(strchr(cmdArg[0], ' ') != NULL) var_set = 0;
	if(strchr(cmdArg[0], '=') == NULL) var_set = 0;
	if(var_set) set_var(cmdArg[0]);
	else execvp(cmdArg[0], cmdArg);
}
