#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <time.h>
#include "SQLDriver.h"


class Transaction{
public:
	Transaction();
	Transaction(SQLDriver *driver);
	Transaction(SQLDriver *driver, std::string name, double amount) throw (UserNotFoundException);
	~Transaction();
	
	int getUserID();
	void setUserID(int id);
	double getAmount();
	void setAmount(double a);
	
	int getTransactionStamp();
	int getTimestamp();
	int getID();
	SQLDriver* getDriver();
	friend class SQLiteDriver;
private:
	int userID;
	int transactionID;
	double amount;
	time_t transactionStamp;
	time_t timestamp;
	std::string title;
	std::string comment;
	SQLDriver *driver;
	bool locked;
};

#endif
