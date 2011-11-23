#include <iostream>
#include "remote_shell.h"
using namespace std;

int remote_shell(string name, string command)
{
    execlp("ssh", "ssh", name.c_str(), command.c_str(), NULL);
}
