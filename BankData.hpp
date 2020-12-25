
#pragma once

#include <iostream> 
#include <list> 
#include <vector> 
#include <map> 
#include <iterator> 
#include <algorithm>
using namespace std;

class Account 
{
	private:
		int id;
		int password;
		int balance;
	public:

		Account(int id_=0, int password_=0, int balance_=0):
			id(id_), password(password_), balance(balance_) {};
		
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
		list <int> AccountList;
		pthread_mutex_t list_lock;

		/* Getters */
		bool CheckList(int AccountNumber);
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