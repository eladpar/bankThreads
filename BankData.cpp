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
    if ( (sem_init(&wrt_lock, 0, 1) != 0) || (sem_init(&rd_lock, 0, 1) != 0) )
				{
					// Error: initialization failed
					//TODO any last words?
				}
    ATMcounter = 1;
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

ATM::ATM(char *argv,int id)
{
    string tmp(argv);
    command = tmp;
    Id = id;
}
