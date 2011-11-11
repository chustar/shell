///#############################################################
///#   San Jose State University                               #
///#   CmpE 142 - Shell program                                #
///#   Author: Abraham Ruiz                                    #
///#           Minh Nguyen                                     #
///#           Chuma Nnaji                                     #
///# file: validation.cpp                                      #
///# Description: Gets input from user and validate grammar.   #
///#                                                           #
///#############################################################


#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <vector>
#include "user_exec.h"


#define COMMAND 'c'
#define TOKEN 't'
#define VARIABLE 'V'

bool fatal_error = false;   //if any unrecoverable error occur, this variable becomes true and shell will terminate

using namespace std;

void validCommand(vector<string>, vector<char> );
int main()
{
    vector<string> commands;
    vector<char> type;
    vector<string>::iterator loc_cmd;
    vector<char>::iterator loctype;
    vector<char>::iterator temp;

    string currentCommand, nextCommand, systemVariable, token;

    while(!fatal_error)
    {
        string inputBuffer;
        string cmd;
        string token;
        bool grammar_error = false;
        int temp1;

        cout << "sh142> ";
        getline(cin,inputBuffer);
        if (inputBuffer == "exit")
            return 0;

        inputBuffer.append(" "); // append space at the end of string; otherwise, last command will not be read
        //cout << endl << "inputBuffer: " << inputBuffer << endl;

        int front_cmd = 0, end_cmd = 0;

        while (inputBuffer[end_cmd])  // scan the entire buffer to separe commands and tokens
        {
            if(inputBuffer[end_cmd] == ' ')
            {
                cmd = inputBuffer.substr(front_cmd,end_cmd - front_cmd);
                front_cmd = end_cmd +1;    // this ignore the white space for the next comand when checking for the rest of
                                           // the buffer
                if(cmd != " ")
                    commands.push_back(cmd);
            }
            end_cmd++;
        }
        for(loc_cmd = commands.begin(); loc_cmd < commands.end(); loc_cmd++)    //scan and validate each element in the command vector
        {
            //cout << "\nvector: " << *loc << endl;         //print content of vector command
            if (*loc_cmd == "&&" || *loc_cmd == "||" || *loc_cmd == "|" || *loc_cmd == "<" || *loc_cmd == ">")
                type.push_back(TOKEN);               //element is a token
            else if (*loc_cmd == "=")
                type.push_back(VARIABLE);            //element if a system variable
            else
                type.push_back(COMMAND);            //element is a command
        }

        for(loctype=type.begin(), loc_cmd=commands.begin(); loctype < type.end(); loctype++, loc_cmd++)
        {
           //  cout << endl << "type: " << *loctype << endl ;   // print content of vector type
            if (*loctype == COMMAND)
            {
                currentCommand = *loc_cmd;
            //    cout << endl << "currentCommand " << currentCommand << endl;  //uncomment later for debugging
                temp = loctype + 1;                                       // get the next element of the vector to check
                                                                          // for grammar error
             //   cout << endl << "temp " << *temp << endl;
                if(*temp == COMMAND)
                {
                    cout << endl << "Grammar error. Operator expected after "<< currentCommand << endl;
                    grammar_error = true;
                    break;                                                     // no command found after token, exit for loop
                }
                //else
                    //validCommand(currentCommand);
            }
            else if (*loctype == VARIABLE)
            {
                systemVariable = *loc_cmd;
                cout << endl << "systemVariable " << systemVariable << endl;
            }
            else if (*loctype == TOKEN)
            {
                token = *loc_cmd;
                cout << endl << "operator: " << token << endl;
            }

        }
        if (!grammar_error){

            temp1 = user_exec(commands,type);          // send vector command to user_exec.cpp
            validCommand(commands,type);

        }
             commands.clear();    //flush content of command vector
             type.clear();       //flush content of type vector
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
