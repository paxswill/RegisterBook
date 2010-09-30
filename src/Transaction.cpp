#include <Transaction.h>

Transaction::Transaction(){
	driver = NULL;
	locked = false;
	userID = -1;
	amount = 0.0;
	transactionStamp = time(NULL);
	timestamp = transactionStamp;
	transactionID = -1;
}

Transaction::Transaction(SQLDriver *driver){

}

Transaction::Transaction(SQLDriver *d, std::string name, double a){
	driver = d;
	User *user  = d->getUser(name);
	if(user){
		userID = user->getID();
		
	}else{
		return NULL;
	}
}

Transaction::~Transaction(){
	//Nothing to free, yet
}

int Transaction::getUserID(){
	return userID;
}

void Transaction::setUserID(int id){
	if(!locked)
	userID = id;
}

double Transaction::getAmount(){
	return amount;
}

void Transaction::setAmount(double a){
	amount = a;
}

int Transaction::getTransactionStamp(){
	return transactionStamp;
}

int Transaction::getTimestamp(){
	return timestamp;
}

int Transaction::getID(){
	return transactionID;
}

SQLDriver* Transaction::getDriver(){
	return driver;
}
