#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "var.h"

#ifndef VAR_CPP
#define VAR_CPP

using namespace std;
int load_vars() {
	int res;
	ifstream file;
	file.open(".sh142", ios::in);
	if (file.bad()) {
		fprintf(stderr, "File not yet created...\n");
		return 1;
	}

	string line;
	while(file.good()) {
		getline(file, line);
		res = set_var(line.c_str());
	}
	file.close();
	return res;		
}

int set_var(const char *var) {
	int res;
	res = putenv((char*)var);
	cout << var << " failed" << endl;
}
#endif
