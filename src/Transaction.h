#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <time.h>
#include "SQLDriver.h"
class Transaction{
public:
	Transaction();
	Transaction(SQLDriver *driver);
	Transaction(SQLDriver *driver, std::string name, double amount);
	~Transaction();
	
	int getUserID();
	void setUserID(int id);
	double getAmount();
	void setAmount(double a);
	
	int getTransactionStamp();
	int getTimestamp();
	SQLDriver* getDriver();
private:
	int userID;
	double amount;
	time_t transactionStamp;
	time_t timestamp;
	SQLDriver *driver;
	bool locked;
};

#endif
