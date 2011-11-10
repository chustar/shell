#include <iostream>

using namespace std;

int exec_env = 0;
int negate = false;

int user_exec(vector<string> var), vector<char> types {
	int child = 0;
	for(int i = 0; i < var.length(); i++) {
		if(types[i] == 'O') {
			resolve_exec();
			set_exec(var[i]);
		} else if(is_cmd(var[i])) {
			child = fork_exec_bg(var[i].str);
		} else {
			resolve_exec(child);
		}
	}
	resolve_exec(child);
}

bool is_token(string v) {
	
}

bool set_exec(string v) {
	int res = 0;
	switch(v.type) {
		case NOT:
			negate = false;
		break;
		case AND:
			res = wait(child);
			if (res == 0 || negate) {
				
			} else if(res != 0 || negate) {

			}
		break;
		case OR:

		break;
		
	}
}

int fork_exec_bg(string v, char type) {

}

bool resolve_exec(int child) {
	if(child != 0) {
		switch(exec_env) {
			case AND:

			break;
			case OR:

			break;
			case STREAM_IN:

			break;
			case STREAM_OUT:

			break;
			case STREAM_ERR:

			break;
			case PIPE:

			break;
			case BACKGROUND:

			break;
		}
	}
}
