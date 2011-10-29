#include <stdio.h>
#include <stdlib.h>

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
	
	contents = (char*) malloc(sizeof(char) * block_size);
	while(getline(&contents, &block_size, fp) != -1) {
		res = putenv(contents);
	}
	return res;		
}


int main() {
	return load_vars();
}
