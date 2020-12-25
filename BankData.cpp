#include "BankData.hpp"  

int Account::getId()
{
    return id;
}

int Account::getPassword()
{
    return password;
}

int Account::getBalance()
{
    return balance;
}

void Account::setId(int id_)
{
    this->id = id_;
}

void Account::setPassword(int password_)
{
    this->password = password_;
}

void Account::setBalance(int balance_)
{
    this->balance = balance_;
}

BankData::BankData()
{
    if (pthread_mutex_init(&list_lock, NULL) != 0) 
    { 
        cerr << "mutex init has failed" << endl;
        //TODO any last words?
        //exit(1);
    } 

    ATMcounter = 1;
}

bool BankData::CheckList(int AccountNumber)
{
    list<int>::iterator it = find(AccountList.begin(), AccountList.end(), AccountNumber); 
    if (it == AccountList.end()) 
    { 
        return false;
    } 
    else
    {
        return true;
    }
    
}

int BankData::get_ATMcounter()
{
    return ATMcounter;
}

void BankData::promote_ATMcounter()
{
    ATMcounter++;
}

BankData::~BankData()
{

}
<<<<<<< HEAD

ATM::ATM(char *argv,int id)
{
    string tmp(argv);
    command = tmp;
    Id = id;
}
=======
>>>>>>> master
