#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "var.c"

int parse_exec(char **cmdArg) {
	//check if its a variable set, otherwise just execute it	
	int var_set = 1;
	char *var, *ptr, *replace, *head, *tail, *res;
	int diff = 0;
	if(strchr(cmdArg[0], ' ') != NULL) var_set = 0;
	if(strchr(cmdArg[0], '=') == NULL) var_set = 0;
	if(var_set) set_var(cmdArg[0]);
	else {
		ptr = strchr(cmdArg[0], '$');
		if(ptr != NULL) {
			diff = cmdArg[0] - ptr;
			replace = (char*)malloc(sizeof(char) * strlen(ptr));
			var = replace;
			while(*ptr != ' ' || *ptr != '\n' || *ptr != '\t' || *ptr != 0) {
				*var = *ptr;
				var++;
				ptr++;	
			}
			*var = 0;
			var = getenv(replace);
			res = (char*)malloc(sizeof(char) * (strlen(cmdArg[0]) + strlen(var)));
			strncpy(res, cmdArg[0], (size_t)diff);
			strcat(res, var);
			strcat(res, ptr);
			execvp(res, cmdArg);
		} else {
			execvp(cmdArg[0], cmdArg);
		}
	}
}

