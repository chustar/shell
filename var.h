#include <iostream>
using namespace std;
#ifndef VAR_H
#define VAR_H
int load_vars();
int find_replace_files(string&);
int find_replace_vars(string&);
string get_var(string);
int set_var(string, string);
#endif
