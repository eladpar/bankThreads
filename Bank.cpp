
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "BankData.hpp"
#include <unistd.h>


#define lock pthread_mutex_lock
#define unlock pthread_mutex_unlock
#define Acc Bank.Accounts.at(AccountNumber) 
#define TargetAcc Bank.Accounts.at(Target_Account) 

#define WRONG_PASSWORD 1
#define ILLEGAL_WITHDRAW 2
#define SUCCESS 3
#define ACCOUNT_DOESNT_EXIST 4

#define CORRECT_PASS 1
#define WRONG_PASS 0
#define NO_ACCOUNT -1

using namespace std;
/* Global Variables */
BankData Bank;
pthread_t *threads;
void BankReadLock()
{
    down(&Bank.rd_lock);
    Bank.rd_count++;
    if (Bank.rd_count == 1)
        down(&Bank.wrt_lock);
    up(&Bank.rd_lock);
}

void BankReadUnlock()
{
    down(&Bank.rd_lock);
    Bank.rd_count--;
    if (Bank.rd_count == 0)
        up(&Bank.wrt_lock);
    up(&Bank.rd_lock);
}

int isExist(int AccountNumber, int atmID)
{
   try
   {
        Account tmp = Acc;
   }
   catch(...)
   {
        cerr << "Error " << atmID <<": Your transaction failed – account id " << AccountNumber << " does not exist" << endl;
        return NO_ACCOUNT;
   }
   return SUCCESS;
}

int isCorrectPassword(int AccountNumber, int password_in_review, int atmID)
{
    if(Acc.getPassword() == password_in_review)
        return SUCCESS; 
    else
    {
        cerr << "Error "<< atmID << ": Your transaction failed – password for account id " << AccountNumber << " is incorrect" << endl;
        return WRONG_PASSWORD;
    }
}



int isCorrectPasswordelad(int AccountNumber, int password_in_review, int atmID)
{
    int correct_password;
    try
    {
        down(&Acc.rd_lock);
        Acc.rd_count++;
        if (Acc.rd_count == 1)
            down(&Acc.wrt_lock);
        up(&Acc.rd_lock);

        if ( password_in_review !=  Acc.getPassword() )
        {
            lock(&Bank.log_lock);
            cerr << "Error "<< atmID << ": Your transaction failed – password for account id " << AccountNumber << " is incorrect" << endl;
            unlock(&Bank.log_lock);
            correct_password = WRONG_PASS;
        }
        else
        {
            correct_password = CORRECT_PASS;
        }

        down(&Acc.rd_lock);
        Acc.rd_count--;
        if (Acc.rd_count == 0)
            up(&Acc.wrt_lock);
        up(&Acc.rd_lock);
    }
    catch (...)
    {
        cerr << "Error " << atmID <<": Your transaction failed – account id " << AccountNumber << " does not exist" << endl;
        correct_password = NO_ACCOUNT;
    }
    return correct_password;

}

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
	    free(threads);
        exit(1);
    }
    if (file.is_open())
    {
        while ( getline (file,line) )
        {
            string delimiter = " ";
            string Action = line.substr(0, line.find(delimiter)); //searching for first delimiter and cutting string before
            line.erase(0, line.find(delimiter) + delimiter.length());
            string AccountNumber_ = line.substr(0, line.find(delimiter)); 
            line.erase(0, line.find(delimiter) + delimiter.length());
            string Password_ = line.substr(0, line.find(delimiter)); 
            line.erase(0, line.find(delimiter) + delimiter.length());
            string Amount_ = line.substr(0, line.find(delimiter)); 
            
            int AccountNumber = stoi(AccountNumber_);
            int Password = stoi(Password_);
            int Amount = stoi(Amount_);

            //TODO sleep(1)

             cout << "Action is : " << Action << " Account Number is: " << AccountNumber 
                                << " Password is: " << Password << " Amount is: " << Amount << endl;
            if (Action == "O") //open account // 
            {
                Account temp_account(AccountNumber, Password, Amount, 0);
                down(&Bank.wrt_lock);
                if (isExist(AccountNumber, atm.Id) == SUCCESS)
                {
                    Bank.Accounts.insert(pair<int, Account>(temp_account.getId(), temp_account)); // TODO lock this ??
                    cerr << atm.Id <<": New account id is "<< AccountNumber << " with password " 
                            << Password << " and initial balance " << Amount << endl;
                }
                up(&Bank.wrt_lock);
            }
            else if (Action == "D") //deposit
            {
                /* Open of bank reader lock */
                BankReadLock();

                if(ACCOUNT_DOESNT_EXIST)
                {
                   up(&Bank.rd_lock); // TODO hana
                   // continue;
                }
                            
                down(&Acc.wrt_lock);

                /* Close of bank reader lock*/
                BankReadUnlock();

                
                Acc.setBalance(Acc.getBalance()+Amount);
                lock(&Bank.log_lock); //
                cerr << atm.Id << ": Account " << Acc.getId() << " new balance is " << Acc.getBalance() << " after " << Amount << " $ was deposited " << endl;
                unlock(&Bank.log_lock); //
                up(&Acc.wrt_lock);

            }


            else if (Action == "W") // withdraw
            {
                try 
                {
                    down(&Acc.wrt_lock);
                    if(Password != Acc.getPassword())
                    {
                        throw(1);
                    }
                    if(Amount > Acc.getBalance())
                    {
                        throw(2);
                    }
                    Acc.setBalance(Acc.getBalance()-Amount);
                    throw(3);

                    up(&Acc.wrt_lock);

                }
                catch(int a)
                {

                    if (a==1)
                    {
                        cerr << "Error " << atm.Id << " Your transaction failed – password for account id " << Acc.getId()  << " is incorrect" << endl;

                    }
                    else if(a==2)
                    {
                        cerr << "Error " << atm.Id << " Your transaction failed – account id " << Acc.getId()  << " balance is lower than " << Amount << endl;
                    }
                    else if(a==3)
                    {
                        cerr << atm.Id << ": Account " << Acc.getId() << " new balance is " << Acc.getBalance() << " after " << Amount << " $ was withdrew " << endl;
                    }
                    up(&Acc.wrt_lock);
                }
                catch(...)
                {
                    cerr << "Error " << atm.Id << ": Your transaction failed – account id " << AccountNumber << " does not exist" << endl;
                }
            }
            else if (Action == "B") // balance
            {
                int curr_password;
                int curr_balance;
                try
                {
                    down(&Acc.rd_lock);
                    Acc.rd_count++;
                    if (Acc.rd_count == 1)
                        down(&Acc.wrt_lock);
                    up(&Acc.rd_lock);

                    sleep(1);
                    curr_password = Acc.getPassword();
                    curr_balance = Acc.getBalance();

                    if ( Password != curr_password )
                    {
                        lock(&Bank.log_lock);
                        cerr << "Error "<< atm.Id << ": Your transaction failed – password for account id " << AccountNumber << " is incorrect" << endl;
                        unlock(&Bank.log_lock);
                    }
                    else
                    {
                        lock(&Bank.log_lock);
                        cerr << atm.Id << ": Account " << AccountNumber << " balance is " << curr_balance << endl;
                        unlock(&Bank.log_lock);
                    }

                    down(&Acc.rd_lock);
                    Acc.rd_count--;
                    if (Acc.rd_count == 0)
                        up(&Acc.wrt_lock);
                    up(&Acc.rd_lock);
                }
                catch (...)
                {
                    cerr << "Error " << atm.Id <<": Your transaction failed – account id " << AccountNumber << " does not exist" << endl;
                }

            }
            else if (Action == "T") // transfer
            {
                line.erase(0, line.find(delimiter) + delimiter.length());
                string The_Real_Amount_ = line.substr(0, line.find(delimiter)); 
                int Target_Account = Amount;
                Amount = stoi(The_Real_Amount_);

                try
                {
                    down(&Acc.wrt_lock);
                    if(Password != Acc.getPassword())
                    {
                        throw(1);
                    }
                    if(Amount > Acc.getBalance())
                    {
                        throw(2);
                    }
                    try
                    {
                        down(&TargetAcc.wrt_lock);
                    }
                    catch (...)
                    {
                        cerr << "Error " << atm.Id << ": Your transaction failed – account id " << Target_Account << " does not exist" << endl;
                        up(&Acc.wrt_lock);

                    }
                    Acc.setBalance(Acc.getBalance()-Amount);
                    TargetAcc.setBalance(TargetAcc.getBalance()+Amount);

                    throw(3);

                }

                catch(int a)
                {

                    if (a==1)
                    {
                        cerr << "Error " << atm.Id << " Your transaction failed – password for account id " << Acc.getId()  << " is incorrect" << endl;

                    }
                    else if(a==2)
                    {
                        cerr << "Error " << atm.Id << " Your transaction failed – account id " << Acc.getId()  << " balance is lower than " << Amount << endl;
                    }
                    else if(a==3)
                    {
                        cerr << atm.Id << ": Transfer " << Amount << " from account " << AccountNumber << " to account " << Target_Account 
                        << " new account balance is " << Acc.getBalance() << " new target account balance is " << TargetAcc.getBalance() << endl;                    
                    }
                    up(&TargetAcc.wrt_lock);
                    up(&Acc.wrt_lock);
                }
                catch(...)
                {
                    cerr << "Error " << atm.Id << ": Your transaction failed – account id " << AccountNumber << " does not exist" << endl;
                }

            }
            else if (Action == "Q") // quit account
            {
                int num;
                int balace;
                down(&Bank.wrt_lock);
                if (isExist(AccountNumber, atm.Id) == SUCCESS)
                {
                    down(&Acc.wrt_lock);
                    if (isCorrectPassword(AccountNumber, Password, atm.Id) == SUCCESS)
                    {
                        balace = Acc.getBalance();
                        num = Bank.Accounts.erase(AccountNumber);
                        if (num != 1)
                            cout << "ERRORRRRRRRRRRRR in Q" << endl; // TODO [DEBUG]
                        cerr << atm.Id << ": Account "<< AccountNumber << " is now closed. Balance was " << balace << endl;
                        up(&Acc.wrt_lock);
                    }
                    up(&Acc.wrt_lock);
                    
                }
                up(&Bank.wrt_lock);
            }
            else
            {
                cerr << "No Such Action " << Action << endl; // TODO [DEBUG?]
            }

        }
        file.close();
    }

    else cerr << "Unable to open file"; 

    pthread_exit(NULL);
}


//**************************************************************************************
// function name: ChargeCommissions
// Description: charge commissions every half sec
//**************************************************************************************

//**************************************************************************************
// function name: ChargeCommissions
// Description: charge commissions every half sec
//**************************************************************************************

void* ChargeCommissions (void* nothing)
{
    // while(1)
    // {
    //     // sleep(1);
        double commission = (25 + (rand()%(63 -25 + 1)) );
        cout << "commission is: " << commission << endl;
    // // }
    return(NULL);
}







//**************************************************************************************
// function name: main
// Description: main function
//**************************************************************************************
int main(int argc, char **argv)
{
    if (argc < 3) cerr << "illegal arguments" << endl;
    freopen( "log.txt", "w", stderr );
    int NumATM = atoi(argv[1]);
    //int *ptr[NumATM];
    vector <ATM> ATM_vector;
    int ATMid = 1;
    for( int t = 0; t < NumATM; t++)
    {
        ATM tmp(argv[t+2],ATMid);
        ATM_vector.push_back(tmp);
        ATMid++;
    }

    threads = (pthread_t*)malloc(sizeof(pthread_t) * (NumATM+1) );
    //pthread_t threads[NumATM]; // TODO malloc?
    int rc,t;
    for (t = 0; t < NumATM; t++)
    {
        rc = pthread_create(&threads[t], NULL, ReadInput, (void*)&ATM_vector[t]);
        if (rc)
        {
            cerr << "ERROR; return code from pthread_create() is " << rc << endl;
            //TODO any last words?
	    free(threads);
            exit(-1);
        }
    }
    rc = pthread_create(&threads[NumATM], NULL, ChargeCommissions, NULL); // Pthread YOSSI - this is the thread of the bank: every half sec chrghes amlot (HAVE TO CHANGE THE FUNC)
        if (rc)
        {
            cerr << "ERROR; return code from pthread_create() is " << rc << endl;
            //TODO any last words?
	    free(threads);
            exit(-1);
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
	    //TODO any last words?
	    free(threads);
        }   
    }

    pthread_exit(NULL);
    
   //TODO any last words?
	free(threads); 
    exit(0);
}

