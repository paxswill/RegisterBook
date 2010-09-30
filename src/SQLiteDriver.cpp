#include "SQLiteDriver.h"

void SQLiteDriver::open(std::string name){
	//Open a database file, creating if needed
	int status;
	status = sqlite3_open_v2(name.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
}

void SQLiteDriver::close(){
	
}

bool SQLiteDriver::commitTransaction(Transaction *t){
	
}

Transaction* SQLiteDriver::makeTransaction(){
	
}

User* SQLiteDriver::makeUser(){
	
}

//Transaction access
std::set<Transaction*> SQLiteDriver::listTransactions(){
	
}

void SQLiteDriver::setTransaction(Transaction *t){
	
}

int SQLiteDriver::countTransactions(){
	
}

//User Access
std::set<User*> SQLiteDriver::listUsers(){
	
}

void SQLiteDriver::setUser(User *u){
	
}

int SQLiteDriver::countUsers(){
	
}

User* SQLiteDriver::getUser(std::string name) throw (UserNotFoundException){
	
}

User* SQLiteDriver::getUser(int userID){
	
}
