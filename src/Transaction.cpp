#include <Transaction.h>

Transaction::Transaction(){
	driver = NULL;
	locked = false;
	userID = -1;
	amount = 0.0;
	transactionStamp = time(NULL);
	timestamp = transactionStamp;
}

Transaction::Transaction(SQLDriver *driver){

}

Transaction::Transaction(SQLDriver *driver, std::string name, double amount){

}

Transaction::~Transaction(){

}

int Transaction::getUserID(){

}

void Transaction::setUserID(int id){

}

double Transaction::getAmount(){

}

void Transaction::setAmount(double a){

}

int Transaction::getTransactionStamp(){

}

int Transaction::getTimestamp(){

}

SQLDriver* Transaction::getDriver(){

}
