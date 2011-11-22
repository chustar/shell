///#############################################################
///#   San Jose State University                               #
///#   CmpE 142 - Shell program                                #
///#   Authors: Abraham Ruiz                                   #
///#           Minh Nguyen                                     #
///#           Chuma Nnaji                                     #
///#   File: validation.cpp                                    #
///#   Description: Gets and validates input from user.        #
///#                Determinates which string is a command,    #
///#                operator, for loop, remote shell or system #
///#                variable                                   #
///#############################################################


#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <vector>
#include <stdlib.h>
#include "user_exec.h"
#include "var.h"
#include "remote_shell.h"
#include "launch_process.h"

#define COMMAND 'C'
#define TOKEN 'T'
#define VARIABLE 'V'


void validCommand(vector<string>, vector<char> );
void for_loop(string );
void remote_shell(string );
void parser(string );
bool checkchar(char );

bool fatal_error = false;   //if any unrecoverable error occur, this variable becomes true and shell will terminate
vector<string> commands;
vector<string>::iterator loc_cmd;
vector<char> type;
vector<char>::iterator loctype;
vector<char>::iterator next_type;

using namespace std;

int main()
{

	init_shell();

    while(!fatal_error){
        string inputBuffer;

        cout << "sh142> ";
        getline(cin,inputBuffer);
        if (inputBuffer == "exit")
            return 0;
        if(inputBuffer.find("$?") == string::npos){
            find_replace_files(inputBuffer);    //call a function to replace data files as needed
            find_replace_vars(inputBuffer);   //call a function to replace variables as needed
        }
            inputBuffer.append(" "); // append space at the end of string; otherwise, last command will not be read

        if  (inputBuffer.find("CPUMAX") != string::npos || inputBuffer.find("MEMMAX") != string::npos
            || inputBuffer.find("TIMEMAX") != string::npos)
        {
            string var = inputBuffer.substr(0,inputBuffer.find('='));
            string value = inputBuffer.substr(inputBuffer.find('=')+1,inputBuffer.size());
            cout << endl << "shell variable " << var << endl;
            cout << endl << "shell value: " << value << endl;
            set_var(var,value);                                  // ***uncomment ***


        }

        else if (inputBuffer.find(':') != string::npos && inputBuffer.find('=') != string::npos){
            cout << endl << "Cannot determinate is input is a shell variable or shell remote access.... " << endl;
            cout << "Please try again" << endl;
        }

        else if (inputBuffer.find(':') != string::npos && inputBuffer.find('#') == string::npos)
        {
            remote_shell(inputBuffer);
        }
        else if(inputBuffer.find('=') != string::npos && inputBuffer.find("for") == string::npos) // input is a shell variable
        {
            string var = inputBuffer.substr(0,inputBuffer.find('='));
            string value = inputBuffer.substr(inputBuffer.find('=')+1,inputBuffer.size());
            cout << endl << "shell variable " << var << endl;
            cout << endl << "shell value: " << value << endl;
            set_var(var,value);                                  // ***uncomment ***

        }
        else if(inputBuffer.find("for") != string::npos)    // for(initialization; condition; increase)
            for_loop(inputBuffer);      //iteration for one or more commands using user's predefined parameters

        else if (inputBuffer.find("$?") != string::npos)
        {
            /*if(commands.size() == 0)
                cout << "No commands executed" << endl;
            else{
              */  int process_num = atoi((inputBuffer.substr(2,inputBuffer.size())).c_str());
                //cout << endl << "check exit status of process number " << process_num << endl;
            	display_exit_status(process_num);
		//}

        }
        else                                    //input is one or more commands
        {
            parser(inputBuffer);
        }
    }

    return 0;

}

void validCommand(vector<string> _commands, vector<char> _type)
{
    vector<string>::iterator current_cmd;
    vector<char>::iterator loctype;

    for(loctype=_type.begin(), current_cmd=_commands.begin(); loctype < _type.end(); loctype++, current_cmd++)
    {
            if(*loctype == COMMAND)
                cout << endl << "Command " << *current_cmd << endl;
            else if (*loctype == VARIABLE)
                cout << endl << "SystemVariable " << *current_cmd << endl;
            else if (*loctype == TOKEN)
                cout << endl << "Operator: " << *current_cmd << endl;
    }

}

void parser(string inputBuffer)
{
    int front_cmd = 0, end_cmd = 0;             //iterators for input string
    bool grammar_error = false;
    int temp1;
    string cmd, currentCommand, nextCommand, systemVariable, token;
        //cout << endl << "inputBuffer " << "\'" << inputBuffer << "\'" << endl;
        while (inputBuffer[end_cmd])  // scan the entire buffer to separe commands and tokens
        {
           // cout << endl << "inputBuffer[end_cmd] " << inputBuffer[end_cmd] << "inputBuffer[end_cmd + 1] " << inputBuffer[end_cmd + 1]  << endl;
            if(inputBuffer[end_cmd] == ' ' && inputBuffer[end_cmd + 1] != '.' && inputBuffer[end_cmd + 1] != '#'
                && inputBuffer[end_cmd + 1] != '/' && inputBuffer[end_cmd + 1] != '-' ){//exit&& checkchar(inputBuffer[end_cmd + 1])){
                cmd = inputBuffer.substr(front_cmd,end_cmd - front_cmd);
                front_cmd = end_cmd + 1;    // this ignore the white space for the next comand when checking for the rest of
                                            // the buffer
                if(cmd != " ")
                    commands.push_back(cmd);
            //    cout << endl << "***cmd: " << cmd << endl;
            }
        end_cmd++;
    }
        if(commands.size() == 1) {          // there is only one element in the vector
            loc_cmd = commands.begin();
            // check if the only element in the vector is a command, operator, or variable
            if (*loc_cmd == "&" || *loc_cmd == "&&" || *loc_cmd == "||" || *loc_cmd == "|" || *loc_cmd == "<" ||
                    *loc_cmd == ">" || *loc_cmd == "2>" || *loc_cmd == ">>"){
                cout << endl << "Grammar error, missing command from user." << endl;    //input is invalid. There is only
                grammar_error = true;                                                   //one operator, and no command
            }
            else {                       //input is valid
                type.push_back(COMMAND);
            }
        }
        else
        {
            for(loc_cmd = commands.begin(); loc_cmd < commands.end(); loc_cmd++){    //scan and validate each element in the command vector
                //cout << "\nvector: " << *loc << endl;         //print content of vector command
                if (*loc_cmd == "&" || *loc_cmd == "&&" || *loc_cmd == "||" || *loc_cmd == "|" || *loc_cmd == "<" ||
                        *loc_cmd == ">" || *loc_cmd == "2>" || *loc_cmd == ">>")
                    type.push_back(TOKEN);               //element is a token
                else if (*loc_cmd == "=")
                    type.push_back(VARIABLE);            //element if a system variable
                else
                    type.push_back(COMMAND);            //element is a command
            }

            for(loctype=type.begin(), loc_cmd=commands.begin(); loctype < type.end(); loctype++, loc_cmd++){
            //  cout << endl << "type: " << *loctype << endl ;   // print content of vector type
                if (*loctype == COMMAND){
                    currentCommand = *loc_cmd;
                //    cout << endl << "currentCommand " << currentCommand << endl;  //uncomment later for debugging
                    next_type = loctype + 1;                                // get the next element of the vector to check
                                                                            // for grammar error
                // cout << endl << "temp " << *temp << endl;
                    /*   if(*next_type == COMMAND){
                        cout << endl << "Grammar error. Operator expected after "<< currentCommand << endl;
                        grammar_error = true;
                        break;                                        // no command found after token, exit for loop
                    }*/
                }
                else if (*loctype == VARIABLE){
                    systemVariable = *loc_cmd;
                // cout << endl << "systemVariable " << systemVariable << endl;
                }
                else if (*loctype == TOKEN){
                    token = *loc_cmd;
                    //cout << endl << "operator: " << token << endl;
                }

            }
        }
        if (!grammar_error){

              //validCommand(commands,type);
            user_exec(commands,type);          // send vector command to user_exec.cpp   ***uncomment ***

        }
            commands.clear();    //flush content of command vector
            type.clear();       //flush content of type vector

}
void for_loop(string input)
{
    vector<string> cmd_group;
    vector<string>::iterator current_cmd;
    bool for_error = false;
    string for_parameter = input.substr(input.find('(')+1); // get for loop parameters
    for_parameter = for_parameter.substr(0,for_parameter.find(')'));
    string initialization = for_parameter.substr(0,for_parameter.find_first_of(';'));
    string condition = for_parameter.substr(for_parameter.find(';')+1);
    condition = condition.substr(0,condition.find(';'));
    string increase = for_parameter.substr(for_parameter.find_last_of(';')+1);
    //cout << endl << "initialization: " << initialization.substr(initialization.find_first_of('=')+1,initialization.size()) << endl;
    //cout << endl << "condition: " << condition << endl;
   // cout << endl << "increase: " << increase << endl;
    int var = atoi(initialization.substr(initialization.find_first_of('=')+1,initialization.size()).c_str());
    int cond_var, for_case;
    if(condition.find("<") != string::npos && increase.find("++") != string::npos){
        if(condition.find("=") != string::npos){
            cond_var = atoi(condition.substr(condition.find_first_of("=")+1,condition.size()).c_str());
            for_case = 4;
        }
        else{
            cond_var = atoi(condition.substr(condition.find_first_of("<")+1,condition.size()).c_str());
            for_case = 0;
        }
    }
    else if(condition.find("<") != string::npos && increase.find("--") != string::npos){
        if(condition.find("=") != string::npos){
            cond_var = atoi(condition.substr(condition.find_first_of("=")+1,condition.size()).c_str());
            for_case = 5;
        }
        else{
            cond_var = atoi(condition.substr(condition.find_first_of("<")+1,condition.size()).c_str());
            for_case = 1;
        }
    }
    else if(condition.find(">") != string::npos && increase.find("++") != string::npos){
        if(condition.find("=") != string::npos){
            cond_var = atoi(condition.substr(condition.find_first_of("=")+1,condition.size()).c_str());
            for_case = 6;
        }
        cond_var = atoi(condition.substr(condition.find_first_of(">")+1,condition.size()).c_str());
        for_case = 2;
    }
    else if(condition.find(">") != string::npos && increase.find("--") != string::npos){
        if(condition.find("=") != string::npos){
            cond_var = atoi(condition.substr(condition.find_first_of("=")+1,condition.size()).c_str());
            for_case = 7;
        }
        else{
            cond_var = atoi(condition.substr(condition.find_first_of(">")+1,condition.size()).c_str());
            for_case = 3;
        }
    }
    else{
        cout << endl << "Invalid for loop parameter. Please try again..." << endl;
        for_error = true;
    }
    string for_loop_commands = "0";
    while (for_loop_commands != "forend" && !for_error){
        getline(cin,for_loop_commands);
        if(for_loop_commands != "forend"){
            for_loop_commands.append(" "); // append space at the end of string; otherwise, last command will not be read
            cmd_group.push_back(for_loop_commands);
        }
    }

    switch (for_case)
    {
        case 0:
            for(int n = var; n < cond_var; n++){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;
        case 1:
            for(int n = var; n < cond_var; n--){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;

        case 2:
            for(int n = var; n > cond_var; n++){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;

        case 3:
            for(int n = var; n > cond_var; n--){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;
        case 4:
            for(int n = var; n <= cond_var; n++){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;
        case 5:
            for(int n = var; n <= cond_var; n--){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;
        case 6:
            for(int n = var; n >= cond_var; n++){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;
        case 7:
            for(int n = var; n >= cond_var; n--){
                for (current_cmd = cmd_group.begin(); current_cmd < cmd_group.end(); current_cmd++){
                    parser(*current_cmd);
                }
            }
            break;
    }

    commands.clear();    //flush content of command vector
}

void remote_shell (string input)
{
    cout << endl << "Starting remote shell..." << endl;
    string remote_name = input.substr(0,input.find(':'));
    string command = input.substr(input.find(':')+1,input.size());
    remote_shell(remote_name, command);

}
bool checkchar(char c)
{
    if (   c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f'
        || c == 'g' || c == 'h' || c== 'i'  || c == 'j' || c == 'k' || c == 'l'
        || c == 'm' || c == 'n' || c== 'o'  || c == 'p' || c == 'q' || c == 'r'
        || c == 's' || c == 't' || c == 'u' || c == 'v' || c == 'w' || c == 'x'
        || c == 'y' || c == 'z')
            return false;
    else
        return true;

}
