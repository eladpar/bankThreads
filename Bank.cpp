
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "BankData.hpp"

#define lock pthread_mutex_lock
#define unlock pthread_mutex_unlock

#define NumATM 1

using namespace std;
/* Global Variables */
BankData Bank;

void *ReadInput(void *atm_tmp)
{
    //*((string*)thread_file)
    //int input_file = *((int*)thread_file);

    //char* input_file_temp = *((char**)thread_file);
    ATM atm = *((ATM*)atm_tmp);
    ifstream file(atm.command);
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
            
            //TODO sleep(1)

             cout << "Action is : " << Action << " Account Number is: " << AccountNumber 
                                << " Password is: " << Password << " Amount is: " << Amount << endl;
            if (Action == "O") //open account // 
            {
                int AccNum = stoi(AccountNumber);

                lock(&Bank.list_lock);
                if(Bank.CheckList(AccNum) == false)
                {
                    Bank.AccountList.push_back(AccNum);
                }
                else
                {
                    cerr << "Account: " << AccNum << " alreasdy exits" << endl; //TODO SAVE TO LOG
                }
                unlock(&Bank.list_lock);
            }
            else if (Action == "D") //deposit
            {
                /* code */
            }
            else if (Action == "W") // withdraw
            {
                /* code */
            }
            else if (Action == "B") // balance
            {
                /* code */
            }
            else if (Action == "T") // transfer
            {
                /* code */
            }
            else if (Action == "Q") // quit account
            {
                /* code */
            }
            else
            {
                cerr << "No Such Action " << Action << endl;
            }
            

            //Account temp_account(atoi(AccountNumber.c_str()), atoi())
           
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
    freopen( "log.txt", "w", stderr );
    //int NumATM = atoi(argv[1]);
    //int *ptr[NumATM];
    vector <ATM> ATM_vector;
    int ATMid = 1;
    cout << "argv[0] is: " << argv[0] << "argv[1] is: " << argv[1] << "argv[2] is: " << argv[2] << endl;
    for( int t = 0; t < NumATM; t++)
    {
        ATM tmp(argv[t+2],ATMid);
        ATM_vector.push_back(tmp);
        ATMid++;
    }

    pthread_t threads[NumATM]; // TODO malloc?
    int rc,t;
    for (t = 0; t < NumATM; t++)
    {
        rc = pthread_create(&threads[t], NULL, ReadInput, (void*)&ATM_vector[t]);
        if (rc)
        {
            cerr << "ERROR; return code from pthread_create() is " << rc << endl;
            //TODO any last words?
            exit(-1);
        }
        
    }
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