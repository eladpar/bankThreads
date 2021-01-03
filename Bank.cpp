
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

#define WRONG_PASSWORD -1
#define ILLEGAL_WITHDRAW -1
#define SUCCESS 3
#define ACCOUNT_DOESNT_EXIST 4

#define CORRECT_PASS 1
#define NO_ACCOUNT -1

using namespace std;

/* Global Variables */
BankData Bank;
pthread_t *threads;
pthread_t *charger_threads;

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
        lock(&Bank.log_lock); //
        cerr << "Error " << atmID <<": Your transaction failed – account id " 
                                << AccountNumber << " does not exist" << endl;
        unlock(&Bank.log_lock); //
        return NO_ACCOUNT;
   }
   return SUCCESS;
}

int isExistNew(int AccountNumber, int atmID)
{
   try
   {
        Account tmp = Acc;
   }
   catch(...)
   {
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
        lock(&Bank.log_lock); //
        cerr << "Error "<< atmID << ": Your transaction failed – password for account id " 
                                        << AccountNumber << " is incorrect" << endl;
        unlock(&Bank.log_lock); //       
        return WRONG_PASSWORD;
    }
}

int isIllegalWithdraw(int AccountNumber, int Amount, int atmID)
{
    if(Amount > Acc.getBalance())
    {
        lock(&Bank.log_lock); //
        cerr << "Error " << atmID << " Your transaction failed – account id " <<
                             Acc.getId()  << " balance is lower than " << Amount << endl;
        unlock(&Bank.log_lock); //       

        return ILLEGAL_WITHDRAW;
    }
    else
        return SUCCESS;
    
    
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
            usleep(100000);
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

            //  cout << "Action is : " << Action << " Account Number is: " << AccountNumber 
            //                     << " Password is: " << Password << " Amount is: " << Amount << endl;
            if (Action == "O") //open account // 
            {
                Account temp_account(AccountNumber, Password, Amount, 0);
                down(&Bank.wrt_lock);
                sleep(1);
                if (isExistNew(AccountNumber, atm.Id) == NO_ACCOUNT)
                {
                    Bank.Accounts.insert(pair<int, Account>(temp_account.getId(), temp_account)); 
                    lock(&Bank.log_lock);
                    cerr << atm.Id <<": New account id is "<< AccountNumber << " with password " 
                            << Password << " and initial balance " << Amount << endl;
                    unlock(&Bank.log_lock);
                }
                else
                {
                    lock(&Bank.log_lock);
                    cerr << "Error " << atm.Id 
                            << ": Your transaction failed – account with the same id exists" << endl;
                    unlock(&Bank.log_lock);
                    
                }
                up(&Bank.wrt_lock);
            }
            else if (Action == "D") //deposit
            {
                try 
                {
                    /* Open of bank reader lock */
                    BankReadLock();

                    if(isExist(AccountNumber,atm.Id)==NO_ACCOUNT)
                    {
                        BankReadUnlock();
                        throw(NO_ACCOUNT);
                    }
                                
                    down(&Acc.wrt_lock);

                    /* Close of bank reader lock*/
                    BankReadUnlock();
                    sleep(1);
                    if(isCorrectPassword(AccountNumber,Password,atm.Id)==WRONG_PASSWORD)
                    {
                        up(&Acc.wrt_lock);
                        throw(WRONG_PASSWORD);
                    }
                    
                    Acc.setBalance(Acc.getBalance()+Amount);
                    lock(&Bank.log_lock); //
                    cerr << atm.Id << ": Account " << Acc.getId() << " new balance is " 
                            << Acc.getBalance() << " after " << Amount << " $ was deposited " << endl;
                    unlock(&Bank.log_lock); //
                    up(&Acc.wrt_lock);
                }
                catch(int reason)
                {
                    if (reason == NO_ACCOUNT)
                    {
                        sleep(1);
                    }
                }
                catch(...)
                {

                }

            }

  
            else if (Action == "W") // withdraw
            {

                try 
                {
                    /* Open of bank reader lock */
                    BankReadLock();

                    if(isExist(AccountNumber,atm.Id)==NO_ACCOUNT)
                    {
                        BankReadUnlock();
                        throw(NO_ACCOUNT);
                    }
                                
                    down(&Acc.wrt_lock);

                    /* Close of bank reader lock*/
                    BankReadUnlock();
                    sleep(1);
                    if(isCorrectPassword(AccountNumber,Password,atm.Id)==WRONG_PASSWORD || 
                                isIllegalWithdraw(AccountNumber,Amount,atm.Id)==ILLEGAL_WITHDRAW)
                    {
                        up(&Acc.wrt_lock);
                        throw(WRONG_PASSWORD);
                    }
                    
                    Acc.setBalance(Acc.getBalance()-Amount);
                    lock(&Bank.log_lock); //
                    cerr << atm.Id << ": Account " << Acc.getId() << " new balance is " 
                                    << Acc.getBalance() << " after " << Amount << " $ was withdrew " << endl;
                    unlock(&Bank.log_lock); //
                    up(&Acc.wrt_lock);                    

                }
                catch(int reason)
                {
                    if (reason == NO_ACCOUNT)
                    {
                        sleep(1);
                    }
                }
                catch(...)
                {

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
                        cerr << "Error "<< atm.Id << ": Your transaction failed – password for account id " 
                                                                << AccountNumber << " is incorrect" << endl;
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
                    cerr << "Error " << atm.Id <<": Your transaction failed – account id " 
                                            << AccountNumber << " does not exist" << endl;
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
                    /* Open of bank reader lock */
                    BankReadLock();

                    if(isExist(AccountNumber,atm.Id)==NO_ACCOUNT || isExist(Target_Account,atm.Id)==NO_ACCOUNT)
                    {
                        BankReadUnlock();
                        throw(NO_ACCOUNT);
                    }
                                
                    down(&Acc.wrt_lock);
                    down(&TargetAcc.wrt_lock);
                    /* Close of bank reader lock*/
                    BankReadUnlock();
                    sleep(1);
                    if(isCorrectPassword(AccountNumber,Password,atm.Id)==WRONG_PASSWORD || 
                                isIllegalWithdraw(AccountNumber,Amount,atm.Id)==ILLEGAL_WITHDRAW)
                    {
                        up(&TargetAcc.wrt_lock);
                        up(&Acc.wrt_lock);
                        throw(WRONG_PASSWORD);
                    }

                    Acc.setBalance(Acc.getBalance()-Amount);
                    TargetAcc.setBalance(TargetAcc.getBalance()+Amount);

                    lock(&Bank.log_lock);
                    cerr << atm.Id << ": Transfer " << Amount << " from account " 
                                << AccountNumber << " to account " << Target_Account 
                                << " new account balance is " << Acc.getBalance() 
                                << " new target account balance is " << TargetAcc.getBalance() << endl; 
                    unlock(&Bank.log_lock);

                    up(&TargetAcc.wrt_lock);
                    up(&Acc.wrt_lock);
                }
                catch(int reason)
                {
                    if (reason == NO_ACCOUNT)
                    {
                        sleep(1);
                    }
                }
                catch(...)
                {
                }
            }
            
            else if (Action == "Q") // quit account
            {
                bool closed = false;
                int num;
                int balace;
                down(&Bank.wrt_lock);
                if (isExist(AccountNumber, atm.Id) == SUCCESS)
                {
                    down(&Acc.wrt_lock);
                    sleep(1); 
                    if (isCorrectPassword(AccountNumber, Password, atm.Id) == SUCCESS)
                    {
                        balace = Acc.getBalance();
                        num = Bank.Accounts.erase(AccountNumber);
                        if (num != 1)
                            cout << "ERRORRRRRRRRRRRR in Q" << endl; // TODO [DEBUG]
                        lock(&Bank.log_lock);
                        cerr << atm.Id << ": Account "<< AccountNumber 
                                    << " is now closed. Balance was " << balace << endl;
                        unlock(&Bank.log_lock);
                        closed =true;
                    }
                    if (closed != true)
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
// Description: create threads who charges commissions every 3 sec
//**************************************************************************************

void* ChargeCommissions (void* nothing)
{
    while(1)
    {
        sleep(3);
        srand((unsigned) time(0));
        double commission = (20 + (rand()%(49 -20 + 1)) ) / 1000.0;
        BankReadLock();
        map<int , Account>::iterator it;
        for (it = Bank.Accounts.begin(); it != Bank.Accounts.end(); it++)
            {
                down(&it->second.wrt_lock);
                double Amount = it->second.getBalance()*commission;
                it->second.setBalance(it->second.getBalance()-(int)Amount);
                Bank.setSelfBalance(Bank.getSelfBalance()+(int)Amount);
                lock(&Bank.log_lock);
                cerr << "Bank: commissions of " << (commission*100) << " % were charged, the bank gained "
                             << (int)Amount << " $ from account " << it->first << endl;
                unlock(&Bank.log_lock);
                up(&it->second.wrt_lock);
            }
        BankReadUnlock();
    }

    return(NULL);
}

//**************************************************************************************
// function name: Printer
// Description: print the exact situation in the bank every 0.5 sec
//*************************************************************************************

void* Printer (void* nothing)
{
    while(1)
    {
        usleep(500000);

        down(&Bank.wrt_lock);
        printf("\033[2J");
        printf("\033[1:1H");
        cout << "Current Bank Status" << endl;
        int AccountNumber; 
        map<int , Account>::iterator it;
        for (it = Bank.Accounts.begin(); it != Bank.Accounts.end(); it++)
            {
                AccountNumber = it->first;
                down(&Acc.rd_lock);
                Acc.rd_count++;
                if (Acc.rd_count == 1)
                    down(&Acc.wrt_lock);
                up(&Acc.rd_lock);

                cout << "Account " << AccountNumber << ": Balance – " 
                        << Acc.getBalance() << " $ , Account Password – " << Acc.getPassword() << endl;

                down(&Acc.rd_lock);
                Acc.rd_count--;
                if (Acc.rd_count == 0)
                    up(&Acc.wrt_lock);
                up(&Acc.rd_lock);
            }
        cout << "The Bank has " << Bank.getSelfBalance() << " $" << endl;
        up(&Bank.wrt_lock);
    }

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

    threads = (pthread_t*)malloc(sizeof(pthread_t) * (NumATM+2) );
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
    // Pthread YOSSI - this is the thread of the bank: every 3 sec chrghes amlot
    rc = pthread_create(&threads[NumATM], NULL, ChargeCommissions, NULL);
        if (rc)
        {
            cerr << "ERROR; return code from pthread_create() is " << rc << endl;
            //TODO any last words?
	       free(threads);
            exit(-1);
        }
        // Pthread MOSHE - this is the thread of the bank: every half sec prints the situation
    rc = pthread_create(&threads[NumATM+1], NULL, Printer, NULL); 
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
    lock(&Bank.log_lock);
    pthread_cancel(threads[NumATM]);
    pthread_cancel(threads[NumATM+1]);
    unlock(&Bank.log_lock);

   //TODO any last words?
	free(threads); 
    exit(0);
}

