#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "var.h"

using namespace std;

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

string find_replace_vars(string &cmd) {
    string start;
    string middle;
    string end;
    size_t pos;
    size_t spos;
    pos = cmd.find_first_of('$', 0);
    if(pos != cmd.npos) {
        spos = ++pos;
        while(isalnum(cmd[spos++]));
        start = cmd.substr(0, pos-1);
        middle = get_var(cmd.substr(pos, spos-pos-1));
        if(spos < cmd.length()){
            end = cmd.substr(spos);
            end = find_replace_vars(end);
           return start + middle + end;
        } else {
            return start + middle;
        }
    } else {
        return cmd;
    }
}

string get_var(string name) {
	char *res = getenv(name.c_str());
	if(res != NULL) return string(res);
	else cerr << name << "Failed" << endl;
}

int set_var(string name, string value) {
	int res;
	res = setenv(name.c_str(), value.c_str(), 1);
	if (res < 0) cout << name << "&" << value << " failed" << endl;
}
