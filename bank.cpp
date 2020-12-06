
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <fstream>
using namespace std;

//**************************************************************************************
// function name: main
// Description: main function
//**************************************************************************************
int main(int argc, char **argv)
{
    int NumATM = atoi(argv[1]);

    for (int i = 2; i < NumATM+2; i ++)
    {
        ifstream file(argv[i]);
        string line;
        if (!file || !file.good()) 
        {
            // File doesn't exist or some other error
            cout << "illegal arguments" << endl;
            //TODO any last words?
            exit(1);
	    }
        if (file.is_open())
        {
            while ( getline (file,line) )
            {
                string delimiter = " ";
                string Action = line.substr(0, line.find(delimiter)); 
                string AccountNumber = line.substr(0, line.find(delimiter)); 
                string Password = line.substr(0, line.find(delimiter)); 
                string InitialAmount = line.substr(0, line.find(delimiter)); 
                cout << "Action is : " << Action << "Account Number is: " << AccountNumber << "Password is: " << Password << "Initial Amount is: " << InitialAmount << endl;
            }
            file.close();
        }
        else cout << "Unable to open file"; 

        
    }
    

    return 0;
}