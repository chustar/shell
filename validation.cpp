///#############################################################
///#   San Jose State University                               #
///#   CmpE 142 - Shell program                                #
///#   Authors: Abraham Ruiz                                   #
///#           Minh Nguyen                                     #
///#           Chuma Nnaji                                     #
///#   File: validation.cpp                                    #
///#   Description: Gets and validates input from user.        #
///#                Determinates which string is a command,    #
///#                operator, or system variable               #
///#############################################################


#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <vector>
#include "user_exec.h"
#include "var.h"

#define COMMAND 'C'
#define TOKEN 'T'
#define VARIABLE 'V'

bool fatal_error = false;   //if any unrecoverable error occur, this variable becomes true and shell will terminate

using namespace std;

void validCommand(vector<string>, vector<char> );
int main()
{
    vector<string> commands;
    vector<string>::iterator loc_cmd;
    vector<char> type;
    vector<char>::iterator loctype;
    vector<char>::iterator next_type;

    string currentCommand, nextCommand, systemVariable, token;

    while(!fatal_error){
        string inputBuffer;
        string cmd;
        bool grammar_error = false;
        int temp1;

        cout << "sh142> ";
        getline(cin,inputBuffer);
        if (inputBuffer == "exit")
            return 0;

        inputBuffer.append(" "); // append space at the end of string; otherwise, last command will not be read
        int front_cmd = 0, end_cmd = 0;             //iterators for input string

        if(inputBuffer.find('=') != string::npos)          // input is a shell variable
        {
            string var = inputBuffer.substr(0,inputBuffer.find('='));
            string value = inputBuffer.substr(inputBuffer.find('=')+1l,inputBuffer.size());
            cout << endl << "shell variable " << var << endl;
            cout << endl << "shell value: " << value << endl;
         //   set_var(var,value);

        }
        else                                    //input is one or more commands
        {
            while (inputBuffer[end_cmd])  // scan the entire buffer to separe commands and tokens
            {
                if(inputBuffer[end_cmd] == ' ' && inputBuffer[end_cmd + 1] != '.'&& inputBuffer[end_cmd + 1] != '#'){
                    cmd = inputBuffer.substr(front_cmd,end_cmd - front_cmd);
                    front_cmd = end_cmd +1;    // this ignore the white space for the next comand when checking for the rest of
                                            // the buffer
                    if(cmd != " ")
                        commands.push_back(cmd);
                }
                end_cmd++;
            }
                if(commands.size() == 1) {          // there is only one element in the vector
                    loc_cmd = commands.begin();
                    // check if the onle element in the vector is a command, operator, or variable
                    if (*loc_cmd == "&" || *loc_cmd == "&&" || *loc_cmd == "||" || *loc_cmd == "|" || *loc_cmd == "<" ||
                    *loc_cmd == ">" || *loc_cmd == "2>"){
                        cout << endl << "Grammar error, missing command from user." << endl;    //input is invalid. There is only
                        grammar_error = true;                                                   //one operator, and no command
                    }
                    else {                       //input is valid
                        //check if the first three letter is the for command
                        //if(for)

                        //else the input is jost a command
                        //else
                        type.push_back(COMMAND);

                    }
                }
                else
                {
                    for(loc_cmd = commands.begin(); loc_cmd < commands.end(); loc_cmd++){    //scan and validate each element in the command vector
                        //cout << "\nvector: " << *loc << endl;         //print content of vector command
                        if (*loc_cmd == "&" || *loc_cmd == "&&" || *loc_cmd == "||" || *loc_cmd == "|" || *loc_cmd == "<" ||
                                *loc_cmd == ">" || *loc_cmd == "2>")
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
                        if(*next_type == COMMAND){
                            cout << endl << "Grammar error. Operator expected after "<< currentCommand << endl;
                            grammar_error = true;
                            break;                                        // no command found after token, exit for loop
                        }
                        //else
                            //validCommand(currentCommand);
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

                user_exec(commands,type);          // send vector command to user_exec.cpp
                validCommand(commands,type);

            }
                commands.clear();    //flush content of command vector
                type.clear();       //flush content of type vector
        }
    }



    return 0;

}

void validCommand(vector<string> _commands, vector<char> _type)
{
/*
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
*/
}
