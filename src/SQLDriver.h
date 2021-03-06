/*
This is the parent class for the SQL drivers.
Forseeable subclasses are those for SQLite and later, MySQL
Also, an XML export function may be tacked onto this as a child
*/
#ifndef SQL_DRIVER_H
#define SQL_DRIVER_H

#include <sqlite3.h>
#include <string>
#include <set>

#include "UserNotFoundException.h"

//Forward declarations
class Transaction;
class User;

class SQLDriver{
public:
	virtual void open(std::string name) = 0;
	virtual void close() = 0;
	virtual bool saveTransaction(Transaction *t) = 0;
	virtual Transaction* makeTransaction() = 0;
	virtual User* makeUser() = 0;
	//Transaction access
	virtual std::set<Transaction*> listTransactions() = 0;
	virtual void setTransaction(Transaction *t) = 0;
	virtual int countTransactions() = 0;
	//User Access
	virtual std::set<User*> listUsers() = 0;
	virtual void setUser(User *u) = 0;
	virtual int countUsers() = 0;
	virtual User* getUser(std::string name) throw (UserNotFoundException) = 0;
	virtual User* getUser(int userID) = 0;
	
};

#endif
