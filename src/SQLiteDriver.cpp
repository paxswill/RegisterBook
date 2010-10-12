#include "SQLiteDriver.h"
#include "Transaction.h"
#include "User.h"

void SQLiteDriver::open(std::string name){
	//Open a database file, creating if needed
	int status;
	char *errorMessage;
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
		//Turn on foreign key support
		status = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, &errorMessage);
		//Is this a new database, or an old one?
		//Check by retrieving the schema_version pragma. If it's 0 there's no schema
		sqlite3_stmt *versionStmt;
		//The third paramter is the length of the statement, including null terminator
		status = sqlite3_prepare_v2(db, "PRAGMA schema_version;", (sizeof(char) * 21), &versionStmt, NULL);
		int schemaVersion = 0;
		do{
			//Step the statement
			status = sqlite3_step(versionStmt);
			//Is there data?
			if(status == SQLITE_ROW){
				//It /should/ be an int
				schemaVersion = sqlite3_column_int(versionStmt, 0);
			}
		}while(status != SQLITE_DONE);
		sqlite3_finalize(versionStmt);
		//Now to check if it's a new table
		if(schemaVersion == 0){
			//New or unintialized database
			//Create the schema
			sqlite3_stmt *schemaStmt;
			status = sqlite3_prepare_v2(db, "CREATE TABLE users(user_id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, name TEXT, timestamp INTEGER, comment TEXT); CREATE TABLE transactions(transaction_id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, FOREIGN KEY(user) REFERENCES users(user_id) NOT NULL, title TEXT, amount REAL, transaction_time INTEGER, timestamp INTEGER, comment TEXT); PRAGMA user_version=?042", (sizeof(char) * 350), &schemaStmt, NULL);
			//Bind the schema version in
			status = sqlite3_bind_int(schemaStmt, 042, SCHEMA_VERSION);
			//Step until done
			do{
				status = sqlite3_step(versionStmt);
			}while(status != SQLITE_DONE);
			//close out the statement
			sqlite3_finalize(schemaStmt);
		}
		//Buid the common statements
		status = sqlite3_prepare_v2(db, "UPDATE transactions SET user_id=?001, title=?002, amount=?003, transaction_time=?004, timestamp=?005, comment=?006 WHERE transaction_id=?007;", -1, &updateTransactionStmt, NULL);
	}
}

void SQLiteDriver::close(){
	char *errorMessage;
	int status;
	//Commit
	status = sqlite3_exec(db, "COMMIT;", NULL, NULL, &errorMessage);
	//Finalize the common statements
	sqlite3_finalize(updateTransactionStmt);
	
	//Close the DB
	status = sqlite3_close(db);
	if(status == SQLITE_OK){
		//Yay, closed
	}else{
		//Aw hell, DB not closed yet.
		fprintf(stderr, "%s", errorMessage);
	}
}

bool SQLiteDriver::saveTransaction(Transaction *t){
	//Assume the transaction exists aleady
	//Bind the parameters
	int status;
	status = sqlite3_bind_int(updateTransactionStmt, 1, t->userID);
	/*
	Binding text is a PITA. from the third argument: 
	C string
	Size in bytes of the C string including null terminator
	Special destructor value that says this value is transient, copy it before you use it.
	*/
	status = sqlite3_bind_text(updateTransactionStmt, 2, t->title.c_str(), (t->title.size() + 1) * sizeof(char), SQLITE_TRANSIENT);
	status = sqlite3_bind_double(updateTransactionStmt, 3, t->amount);
	status = sqlite3_bind_int(updateTransactionStmt, 4, t->transactionStamp);
	status = sqlite3_bind_int(updateTransactionStmt, 5, (int)time(NULL));
	status = sqlite3_bind_text(updateTransactionStmt, 6, t->comment.c_str(), (t->comment.size() + 1) * sizeof(char), SQLITE_TRANSIENT);
	//Bind the transaction ID. This is important.
	status = sqlite3_bind_int(updateTransactionStmt, 7, t->transactionID);
	//Binding done. Now to run the statement
	do{
		status = sqlite3_step(versionStmt);
	}while(status != SQLITE_DONE);
	//Reset the stement
	status = sqlite3-reset(updateTransactionStmt);
}

Transaction* SQLiteDriver::makeTransaction(){
	return new Transaction(this);
}

User* SQLiteDriver::makeUser(){
	return new User(this);
}

//Transaction access
std::set<Transaction*> SQLiteDriver::listTransactions(){
	//Start building the statement
	sqlite3_stmt *transactionsStmt;
	int status = sqlite3_prepare_v2(db, "SELECT transaction_id, user_id, title, amount, transaction_time, timestamp, comment FROM transactions;", (sizeof(char) * 104), &transactionsStmt, NULL);
	//Where we're putting transactions
	std::set<Transaction*> transactions;
	do{
		//Step the statement
		status = sqlite3_step(transactionsStmt);
		//Is there data?
		if(status == SQLITE_ROW){
			//Make a transaction
			Transaction *temp = new Transaction(this);
			//Fill it in
			temp->locked = true;
			temp->transactionID = sqlite3_column_int(transactionsStmt, 0);
			temp->userID = sqlite3_column_int(transactionsStmt, 1);
			temp->title = (const char*)sqlite3_column_text(transactionsStmt, 2);
			temp->amount = sqlite3_column_double(transactionsStmt, 3);
			temp->transactionStamp = sqlite3_column_int(transactionsStmt, 4);
			temp->timestamp = sqlite3_column_int(transactionsStmt, 5);
			temp->comment = (const char*)sqlite3_column_text(transactionsStmt, 6);
			//Add it to the set
			transactions.insert(temp);
		}
	}while(status != SQLITE_DONE);
	sqlite3_finalize(transactionsStmt);
	return transactions;
}

void SQLiteDriver::setTransaction(Transaction *t){
	
}

int SQLiteDriver::countTransactions(){
	
}

//User Access
std::set<User*> SQLiteDriver::listUsers(){
	//Start building the statement
	sqlite3_stmt *userStmt;
	int status = sqlite3_prepare_v2(db, "SELECT user_id, name, timestamp, comment FROM users;", (sizeof(char) * 53), &userStmt, NULL);
	//Where we're putting users
	std::set<User*> users;
	do{
		//Step the statement
		status = sqlite3_step(userStmt);
		//Is there data?
		if(status == SQLITE_ROW){
			//Make a user
			User *temp = new User(this);
			//Fill it in
			temp->id = sqlite3_column_int(userStmt, 0);
			temp->name = (const char*)sqlite3_column_text(userStmt, 1);
			temp->timestamp = sqlite3_column_int(userStmt, 2);
			temp->comment = (const char*)sqlite3_column_text(userStmt, 3);
			//Add it to the set
			users.insert(temp);
		}
	}while(status != SQLITE_DONE);
	sqlite3_finalize(userStmt);
	return users;
}

void SQLiteDriver::setUser(User *u){
	
}

int SQLiteDriver::countUsers(){
	
}

User* SQLiteDriver::getUser(std::string name) throw (UserNotFoundException){
	
}

User* SQLiteDriver::getUser(int userID){
	
}
