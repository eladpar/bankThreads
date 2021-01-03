
#pragma once

#include <iostream> 
#include <list> 
#include <vector> 
#include <map> 
#include <iterator> 
#include <algorithm>
#include <semaphore.h>

#define down sem_wait
#define up sem_post
using namespace std;

class Account 
{
	private:
		int id;
		int password;
		int balance;
	public:
		int rd_count;
		sem_t wrt_lock;
		sem_t rd_lock;
	// add write lock and read lock
		Account(int id_=0, int password_=0, int balance_=0, int rd_count_ =0):
			id(id_), password(password_), balance(balance_), rd_count(rd_count_) 
			{
				if ( (sem_init(&wrt_lock, 0, 1) != 0) || (sem_init(&rd_lock, 0, 1) != 0) )
				{
					// Error: initialization failed
					//TODO any last words?
				}


			};
		
		/* Getters */
		int getId();
		int getPassword();
		int getBalance();

		/* Setters */
		void setId(int id_);
		void setPassword(int password_);
		void setBalance(int balance_);

		
		~Account(){};

};


class BankData  
{
	private:
		int ATMcounter;
		
	public:
		/* Constructor */
		BankData();

		/* Variables */
		map <int , Account> Accounts;
		pthread_mutex_t log_lock;

		sem_t wrt_lock;
		sem_t rd_lock;
		int rd_count;
		
		/* Getters */
		int get_ATMcounter();
		/* Setters */
		void promote_ATMcounter();
		/* Destructor */
		~BankData();

};

class ATM  
{
	private:
		
	public:
		/* Constructor */
		ATM(char *argv,int id);

		/* Variables */
		int Id;
		string command;

		/* Printers */
		//void printer_AccNumberIsTaken(); 
		//void printer_AccOpenSucceeded(int id,int password,int balance);

		/* Destructor */
		~ATM(){};




};

class ChargerThread 
{
	private:

	public:
	ChargerThread(int AccountNumber_, int Commision_):
			AccountNumber(AccountNumber_), Commision(Commision_) {};

	int AccountNumber;
	int Commision;

	~ChargerThread() {};
};