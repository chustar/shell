#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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

int find_replace_files(string &cmd) {
    return 0;
    string start;
	string middle;
	string end;
	size_t pos;
	size_t spos;
	pos = cmd.find_first_of('#', 0);
	if(pos != cmd.npos) {
		start = cmd.substr(0, pos);
		if(cmd[pos++] == '#' && cmd[pos++] == 'D' && cmd[pos++] == ':') {
			spos = pos;
			while(cmd[spos] != 0 && cmd[spos] != ' ') spos++;
			string file = cmd.substr(pos, spos-pos);
			string path = get_var("DATA");
			if(path == "") {
				cerr << "DATA variable not set" << endl;
				return 1;
			}

			middle = "";
			string dir;
			istringstream search(path);
			while(search.good()) {
				getline(search, dir, ':');
				if(dir != "") {
					struct stat st;
					if(dir[dir.length()] != '/') dir.append("/");
                    middle = dir + file;
					//is file in dir?
					if(stat(middle.c_str(), &st) == 0) {
						break;
					} else {
						middle = "";
					}
				}
			}
			if(middle == "") {
				cerr << "File: " << file << "Does not exist in DATA path" << endl;
				return 2;
			}
			if(spos < cmd.length()) {
				end = cmd.substr(spos);
				find_replace_files(end);
				cmd = start + middle + end;
			} else {
				cmd = start + middle;
			}
		}
    }
}

int find_replace_vars(string &cmd) {
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
			find_replace_vars(end);
		    cmd = start + middle + end;
		} else {
			cmd = start + middle;
		}
	}
}

string get_var(string name) {
	char *res = getenv(name.c_str());
	if(res != NULL) return string(res);
	else return "";
}

int set_var(string name, string value) {
	int res;
	res = setenv(name.c_str(), value.c_str(), 1);
	if (res < 0) cout << name << "&" << value << " failed" << endl;
}
