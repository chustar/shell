#include <iostream>
#include <vector>
#include <fstream>
#include "user_exec.cpp"

using namespace std;

int main() {
	vector<string> cmds;
	vector<char> types;

	ifstream file;
	file.open("exec.txt", ios::in);
	string cmd;
	string type;

	while (file.good()) {
		getline(file, cmd);
		if(file.eof()) break;
		getline(file, type);
		cmds.push_back(cmd);
		types.push_back(type[0]);
	}	
	file.close();
	user_exec(cmds, types);	
}
