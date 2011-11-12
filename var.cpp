#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "var.h"

using namespace std;

int load_vars();
string get_var(string);
int set_var(string, string);

int load_vars() {
	int res;
	ifstream file;
	file.open(".sh142", ios::in);
	if (file.bad()) {
		cerr << "File not yet created...\n";
		return 1;
	}

	string name;
	string value;
	while(file.good()) {
		getline(file, name, '=');
		if(file.good()) {
			getline(file, value);
			res = set_var(name, value);
		}
	}
	file.close();
	return res;
}

string get_var(string name) {
	char *res = getenv(name.c_str());
	if(res != NULL) return string(res);
	else cerr << name << " failed" << endl;
}

int set_var(string name, string value) {
	int res;
	res = setenv(name.c_str(), value.c_str(), 1);
	if (res < 0) cout << name << "&" << value << " failed" << endl;
}
