#include "SQLiteDriver.h"

void SQLiteDriver::open(std::string name){
	//Open a database file, creating if needed
	int status;
	status = sqlite3_open_v2(name.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if(status != SQLITE_OK){
		//There was an error opening the DB
		const char *errorMsg = sqlite3_errmsg(db);
		fprintf(stderr, "%s", errorMsg);
		//TODO: Close out the DB and raise an exception
		
	}else{
		//DB opened successfully
		//Turn on extended error codes
		sqlite3_extended_result_codes(db, 1);
		//Is this a new database, or an old one?
		//Check by retrieving the schema_version pragma. If it's 0 there's no schema
		sqlite3_stmt *versionStmt;
		//The third paramter is the length of the statement, including null terminator
		status = sqlite3_prepare_v2(db, "PRAGMA user_version;", (sizeof(char) * 21), versionStmt, NULL);
		//There's a quirk in SQLite < 3.6.23.1, (OS X as of 10.6.5 has 3.6.12, while Macports has 3.7.2)
		bool legacyReset = sqlite3_libversion_number() < 3006023;
		int schemaVersion = 0;
		do{
			//Step the statement
			status = sqlite3_step(versionStmt);
			//Legacy junk
			if(legacyReset && (status != SQLITE3_ROW || status != SQLITE3_DONE)){
				sqlite3_reset(versionStmt);
			}
			//Is there data?
			if(status == SQLITE3_ROW){
				//It /should/ be an int
				schemaVersion = sqlite3_column_int(versionStmt, 0);
			}
		}while(status != SQLITE3_DONE);
		sqlite3_finalize(versionStmt);
		//Now to check if it's a new table
		if(schemaVersion == 0){
			//New or unintialized database
			//Create the schema
			
		}
	}
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
