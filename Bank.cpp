
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "BankData.hpp"

#define NumATM 2
using namespace std;

void *ReadInput(void *thread_file)
{
    string input_file = *((string*)thread_file);
    ifstream file(input_file);
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
            string Action = line.substr(0, line.find(delimiter)); //searching for first delimiter and cutting string before
            line.erase(0, line.find(delimiter) + delimiter.length());
            string AccountNumber = line.substr(0, line.find(delimiter)); 
            line.erase(0, line.find(delimiter) + delimiter.length());
            string Password = line.substr(0, line.find(delimiter)); 
            line.erase(0, line.find(delimiter) + delimiter.length());
            string Amount = line.substr(0, line.find(delimiter)); 
            
            //Account temp_account(atoi(AccountNumber.c_str()), atoi())
            cout << "Action is : " << Action << " Account Number is: " << AccountNumber << " Password is: " << Password << " Amount is: " << Amount << endl;
        }
        file.close();
    }

    else cerr << "Unable to open file"; 

    pthread_exit(NULL);
}

//**************************************************************************************
// function name: main
// Description: main function
//**************************************************************************************
int main(int argc, char **argv)
{
    if (argc < 3) cerr << "illegal arguments" << endl;

    //int NumATM = atoi(argv[1]);
    //int NumATM = 2;
    //pthreds_create (1->3)
    int counter =0;
    //int *ptr[NumATM];
    pthread_t threads[NumATM]; // TODO malloc?
    int rc,t;
    for (t = 0; t < NumATM; t++)
    {
        rc = pthread_create(&threads[t], NULL, ReadInput, (void*)argv[t+2]);
        if (rc)
        {
            cerr << "ERROR; return code from pthread_create() is " << rc << endl;
            //TODO any last words?
            exit(-1);
        }
        
    }
cerr << "hre"<<endl;
    for (int i = 0; i < NumATM; i++)
    {
        try
        {
            pthread_join(threads[i], NULL);
        }
        catch(const std::exception& e)
        {
            cerr << "Caught" <<  e.what() << '\n';
        }
        
        
    }
    
    
    exit(0);
}