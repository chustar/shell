#include <iostream>

using namespace std;

int user_exec(vector<block> var) {
	int child = 0;
	for(int i = 0; i < var.length(); i++) {
		if(is_token(var[i])) {
			resolve_exec();
			set_exec(var[i]);
		} else if(is_cmd(var[i])) {
			child = fork_exec_bg(var[i].str);
		}
		else {
			resolve_exec(child);
		}
	}
}

bool is_token(block v) {

}

bool set_exec(block v) {

}

int fork_exec_bg(block v) {

}

bool resolve_exec(int child) {

}
