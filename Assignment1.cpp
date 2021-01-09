/*
Author: Jacob Cohen, Matthew Younker
File:Assignment1.cpp
Date: 9/27/2019
*/

//libaries
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>

using namespace std;

//struct
struct UPpair
{
    string username;
    string password;
};

//methods
bool nameChecker(string name);
bool passwordChecker(string password);

//commands
void changePassword();
void copyFile(string inputFile, string copyFile1, string copyFile2);
void processes(string username);
void diskSpace(string fileSystem);
void search(string word, string inputFile);
void history();
void logout();
string encryptDecrypt(string toEncrypt);

//ext files
void readCommandFile();
void readLoginFile();
void saveLoginFile();

//main functions
void login();
void typePrompt();
int readCommand(string command);
void executeCommand(int opcode, string parameters);
void upArrow();

//variables
UPpair UPlogin;
string commands[7];
vector<string> commandsHistory;
int _numberOfTimesToRun = 10;
int main()
{
    //background
    int opcode = 0;
    int pid = 0;
    int status = 0; 
    int background = 0;
    int count = 0;
    string command;
    string parameters;

    //read commands in
    readCommandFile();

    //read login file
    readLoginFile();

    /* Authenticate the user */
    login();

    

    /* repeat maximum N times */
    while (count < _numberOfTimesToRun) 
    {        
        //prompt
        typePrompt();    

        //get input
        cin >> command;
        getline(cin, parameters);

        //find index
        opcode = readCommand(command);  

        if (opcode > -1) 
        { 
            /* valid command */
            if (pid = fork() != 0) 
            {
                if (!background) 
                {
                    pid = wait(&status); 
                
                    if (status == 0x0500) 
                    {
                        exit(0);
                    } 
                } 
            } 
            else 
            { 
                executeCommand (opcode, parameters);
                exit(0); 
            } 
        } 
        else 
        { 
            cout << "Invalid command, try again\n"; 
        }

        //inc
        count++;
  } 

  //done
  return 0;
}

//logs the user in
void login()
{
    //variables
    string name;
    string password;

    //prompt
    cout << "> Username: ";

    //get input
    cin >> name;

    //loop while username is not good
    while (nameChecker(name) == false)
    {
        //promot
        cout << "Invalid username" << endl;

        //ask for username again
        cout << "> Username: ";

        //get input
        cin >> name;
    }

    //prompt
    cout << "> Password: ";

    //disable echo 
    system("stty -echo");

    //get input
    cin >> password;

    //check to make sure password and name are good
    while (passwordChecker(password) == false)
    {

        //promot
        cout  << endl << "Invalid password" << endl;

        //ask for username again
        cout << "> Password: ";

        //get input
        cin >> password;
    }

    cout << endl;

    //re-enable echo
    system("stty echo");

    //done
    return;
}

void typePrompt()
{
    cout << "msh>> ";
}

//figures out what command to use and separates command into its parts
int readCommand(string command)
{
    //check to see if command exits
    for(int i = 0; i < 7; i++)
    {
        if(command == commands[i])
        {
            //add to history
            commandsHistory.push_back(commands[i]);

            //done
            return i;
        }
    }

    //done
    return -1;
}


//calls the command
void executeCommand(int opcode, string parameters)
{
    //variables
    istringstream parametersStream(parameters);

    //copy
    string file1;
    string file2;
    string file3 = "";

    //search
    string word;
    string inputFile;

    switch (opcode)
    {
        //password
    case 0:
        changePassword();
        break;

        //copy file
    case 1:

        //split three times
        parametersStream >> file1;
        parametersStream >> file2;
        parametersStream >> file3;

        copyFile(file1, file2, file3);
        break;

        //proccesses
    case 2:
        processes(parameters);
        break;

        //disk space
    case 3:
        diskSpace(parameters);
        break;

        //search
    case 4:

        //split into word and inputFile
        parametersStream >> word;
        parametersStream >> inputFile;

        search(word, inputFile);
        break;
    
        //history
    case 5:
        history();
        break;

        //logout
    case 6:
        exit(5);
        break;

    default:
        //should never hit this
        break;
    }

    //done
    return;
}

//checks to make sure the username is correct
bool nameChecker(string name)
{
    //check
    if (name == UPlogin.username)
    {
        //done
        return true;
    }

    //done
    return false;
}

//checks to make sure password is correct
bool passwordChecker(string password)
{
    //check
    if (UPlogin.password == encryptDecrypt(password))
    {
        //done
        return true;
    }

    //done
    return false;
}

//changes users password
void changePassword()
{
	//variables
	string input;

	//prompt
	cout << "New Password: ";

    //disable echo
    system("stty -echo");

	//get input
	cin >> input;

	//replace in vector
	UPlogin.password = encryptDecrypt(input);

	//save to file
	saveLoginFile();

    //re-enable echo
    system("stty echo");

	//prompt
	cout << endl << "Password successfully changed" << endl;

    //done
    return;
}

void copyFile(string inputFile, string copyFile1, string copyFile2 = "")
{
	//only one file
	if ("" == copyFile2)
    {

		//linux command
		system(("cp " + inputFile + " " + copyFile1).c_str());
	}
	//two files
	else
	{
		//linux command
		system(("cat " + inputFile + " " + copyFile1 + " > " + copyFile2).c_str());
	}

	//done
	return;
}

void processes(string username = "")
{
	//no username provided
	if ("" == username)
	{
		//linux command
		system("ps -aux | less");
	}
	//username provided
	else
	{
		//linux command
        system(("ps -u " + username).c_str());
    }

	//done
	return;
}

void diskSpace(string fileSystem = "")
{
	//no file system provided
	if ("" == fileSystem)
	{
		//linux command
		system("df -k");
	}
	//file system provided
	else
	{
		//linux command
		system(("df -k " + fileSystem).c_str());
	}

	//done
	return;
}

//return line number where word exits
void search(string word, string inputFile)
{
	//linux command
	system(("grep -inR  \""+ word + "\" " + inputFile).c_str());

	//done
	return;
}

//display all commands that the user ran
void history()
{
    //prompt
    cout << "HISTORY" << endl;

	//loop through vector
	for (int i = 0; i < commandsHistory.size(); i++)
	{
		//print out item at current index
		cout << commandsHistory[i] << endl;
	}

    cout << endl;

	//done
	return;
}

//get login infomation from external file
void readLoginFile()
{
    //open file
    fstream file("Assignment1Login.txt", ios::in);

    //read file
    file >> UPlogin.username;
    file >> UPlogin.password;

    //close file
    file.close();

    //done
    return;
}

//Saves the password
void saveLoginFile()
{
	//open file
	fstream file;
    file.open("Assignment1Login.txt", ios::out);

	//write to file
    file << UPlogin.username << " " << UPlogin.password;

    //close file
    file.close();

	//done
	return;
}

//read commands in from external file
void readCommandFile()
{
    //open file
    fstream file("Assignment1Commands.txt", ios::in);
    
    //read file
    for(int i = 0; i < 7; i++)
    {
        //add command to vector
        file >> commands[i];
    }

    //close file
    file.close();

    //done
    return;
}

//this sometimes breaks?
//modified from https://kylewbanks.com/blog/Simple-XOR-Encryption-Decryption-in-Cpp
string encryptDecrypt(string toEncrypt) 
{
	char key = 'K'; 
	string output = toEncrypt;

	for (int i = 0; i < toEncrypt.size(); i++)
    {
		output[i] = toEncrypt[i] ^ key;
    }

	return output;
}
