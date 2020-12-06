
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
    if (argc < 3) cerr << "illegal arguments" << endl;
    int NumATM = atoi(argv[1]);
    for (int i = 2; i < NumATM+2; i ++)
    {
        ifstream file(argv[i]);
        string line;
        if (!file || !file.good()) 
        {
            // File doesn't exist or some other error
            cerr << "illegal arguments" << endl;
            //TODO any last words?
            exit(1);
	    }
        if (file.is_open())
        {
            while ( getline (file,line) )
            {
                string delimiter = " ";
                string Action = line.substr(0, line.find(delimiter)); 
                line.erase(0, line.find(delimiter) + delimiter.length());
                string AccountNumber = line.substr(0, line.find(delimiter)); 
                line.erase(0, line.find(delimiter) + delimiter.length());
                string Password = line.substr(0, line.find(delimiter)); 
                line.erase(0, line.find(delimiter) + delimiter.length());
                string InitialAmount = line.substr(0, line.find(delimiter)); 
                cout << "Action is : " << Action << " Account Number is: " << AccountNumber << " Password is: " << Password << " Initial Amount is: " << InitialAmount << endl;
            }
            file.close();
        }
        else cerr << "Unable to open file"; 

    }
    

    return 0;
}