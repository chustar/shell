#include <iostream>
#include <vector>

#include "user_exec.cpp"

using namespace std;

int main() {
	vector<string> cmds;
	vector<char> types;
	
	cmds.push_back("ls");
	types.push_back('C');

	cmds.push_back("&&");
	types.push_back('T');

	cmds.push_back("cat .sh142");
	types.push_back('C');
	user_exec(cmds, types);	
}
