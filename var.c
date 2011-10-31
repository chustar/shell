#include <stdio.h>
#include <stdlib.h>

#ifndef VAR_C
#define VAR_C
int load_vars();
int set_var(char *);

int load_vars() {
	FILE *fp;
	char *contents;
	size_t block_size = 100;
	int res = 0;

	fp = fopen(".sh142", "r");
	if (fp == NULL) {
		fprintf(stderr, "File not yet created...\n");
		return 1;
	}

	//read and execute every line of the file as an envirnoment var set
	contents = (char*) malloc(sizeof(char) * block_size);
	while(getline(&contents, &block_size, fp) != -1) {
		res = set_var(contents);
	}
	fclose(fp);
	return res;		
}

int set_var(char *var) {
	int res;
	res = putenv(var);
}


#ifdef UNITTEST
int main() {
	return load_vars();
}
#endif
#endif
