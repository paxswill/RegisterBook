//SQLiteDriver

#ifndef SQLITE_DRIVER_H
#define SQLITE_DRIVER_H

//NOTE: This is the version of the schema this driver can work with natively
#define SCHEMA_VERSION 1

#include <sqlite3.h>
#include "SQLDriver.h"

class SQLiteDriver : public SQLDriver{
public:
	virtual void open(std::string name);
	virtual void close();
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
	sqlite3_stmt *checkTransactionStmt;
	sqlite3_stmt *updateTransactionStmt;
	//Utility functions
	static int bind(sqlite3_stmt *stmt, const char *var_name, int var);
	static int bind(sqlite3_stmt *stmt, const char *var_name, double var);
	static int bind(sqlite3_stmt *stmt, const char *var_name, std::string var);
};

#endif
