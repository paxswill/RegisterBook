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
#include "Transaction.h"

class SQLDriver{
public:
	virtual void open(std::string name) = 0;
	virtual void close() = 0;
	virtual bool commitTransaction(Transaction *t) = 0;
	virtual Transaction* makeTransaction() = 0;
	virtual user* makeUser() = 0;
	//Transaction access
	virtual std::set<Transaction*> listTransactions() = 0;
	virtual void setTransaction(Transaction *t) = 0;
	virtual int countTransactions() = 0;
	//User Access
	virtual std::set<User*> listUsers() = 0;
	virtual void setUser(User *u) = 0;
	virtual int countUsers() = 0;
};

#endif
