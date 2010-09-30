//SQLiteDriver

#ifndef SQLITE_DRIVER_H
#define SQLITE_DRIVER_H

#include <sqlite3.h>
#include "SQLDriver.h"

class SQLiteDriver : public SQLDriver{
public:
	virtual void open(std::string name);
	virtual void close();
	virtual bool commitTransaction(Transaction *t);
	virtual Transaction* makeTransaction();
	virtual User* makeUser();
	//Transaction access
	virtual std::set<Transaction*> listTransactions();
	virtual void setTransaction(Transaction *t);
	virtual int countTransactions();
	//User Access
	virtual std::set<User*> listUsers();
	virtual void setUser(User *u);
	virtual int countUsers();
	virtual User* getUser(std::string name) throw (UserNotFoundException);
	virtual User* getUser(int userID);
private:
	sqlite3 *db;
};

#endif
