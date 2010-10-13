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
		status = sqlite3_prepare_v2(db, "UPDATE transactions SET user_id=@label_user_id, title=@label_title, amount=@label_amount, transaction_time=@label_transaction_time, timestamp=@label_timestamp, comment=@label_comment WHERE transaction_id=@label_transaction_id;", -1, &updateTransactionStmt, NULL);
		status = sqlite3_prepare_v2(db, "SELECT transaction_id FROM TRANSACTIONS WHERE transaction_id=@label_transaction_id LIMIT 1;", -1, &checkTransactionStmt, NULL);
		status = sqlite3_prepare_v2(db, "INSERT INTO transactions(transaction_id, user, title, amount, transaction_time, timestamp, comment) VALUES (@label_transaction_id, @label_user_id, @label_title, @label_amount, @label_transaction_time, @label_timestamp, @label_comment);", -1, &insertTransactionStmt, NULL);
	}
}

void SQLiteDriver::close(){
	char *errorMessage;
	int status;
	//Commit
	status = sqlite3_exec(db, "COMMIT;", NULL, NULL, &errorMessage);
	//Finalize the common statements
	sqlite3_finalize(updateTransactionStmt);
	sqlite3_finalize(checkTransactionStmt);
	//Close the DB
	status = sqlite3_close(db);
	if(status == SQLITE_OK){
		//Yay, closed
	}else{
		//Aw hell, DB not closed yet.
		fprintf(stderr, "%s", errorMessage);
	}
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
	int status;
	sqlite3_stmt *workingStatment;
	//Check to see if the statement exists
	workingStatment = checkTransactionStmt;
	status = bind(workingStatment, "label_transaction_id", t->transactionID);
	//Run the statement
	int checkValue = -1;
	do{
		status = sqlite3_step(workingStatment);
		if(!checkError(status)){
			break;
		}
		int tempCheck = sqlite3_column_int(workingStatment, 0);
		checkValue = tempCheck > checkValue ? tempCheck : checkValue;
	}while(status != SQLITE_DONE);
	//Reset the statement
	status = sqlite3_reset(workingStatment);
	//Are we updating or inserting?
	if(checkValue == -1){
		//Inserting
		workingStatment = insertTransactionStmt;
	}else{
		//Updating
		workingStatment = updateTransactionStmt;
	}
	//Bind the parameters
	status = bind(workingStatment, "label_user_id", t->userID);
	status = bind(workingStatment, "label_title", t->title);
	status = bind(workingStatment, "label_amount", t->amount);
	status = bind(workingStatment, "label_transaction_time", (int)t->transactionStamp);
	status = bind(workingStatment, "label_timestamp", (int)time(NULL));
	status = bind(workingStatment, "label_comment", t->comment);
	status = bind(workingStatment, "label_transaction_id", t->transactionID);
	
	//Binding done. Now to run the statement
	do{
		status = sqlite3_step(workingStatment);
		if(!checkError(status)){
			break;
		}
	}while(status != SQLITE_DONE);
	//Reset the stement
	status = sqlite3_reset(updateTransactionStmt);
}

int SQLiteDriver::countTransactions(){
	//No need for a prepared statment here
	int status;
	char **results, *errorMsg;
	int rows, cols;
	status = sqlite3_get_table(db, "SELECT COUNT(transaction_id) FROM transactions;", &results, &rows, &cols, &errorMsg);
	//We should ust have 1 row and 1 column, we're just getting a number, but get the last element just in case
	int numRecords = atoi(results[((rows + 1) * cols) - 1]);
	//Free the results
	sqlite3_free_table(results);
	return numRecords;
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
	//Almost exactly like countTransactions above.
	int status;
	char **results, *errorMsg;
	int rows, cols;
	status = sqlite3_get_table(db, "SELECT COUNT(user_id) FROM users;", &results, &rows, &cols, &errorMsg);
	//We should ust have 1 row and 1 column, we're just getting a number, but get the last element just in case
	int numRecords = atoi(results[((rows + 1) * cols) - 1]);
	//Free the results
	sqlite3_free_table(results);
	return numRecords;
}

User* SQLiteDriver::getUser(std::string name) throw (UserNotFoundException){
	
}

User* SQLiteDriver::getUser(int userID){
	
}

//Private Utility methods

/*
	The bind() methods hide the SQLite3 C API for binding away.
	The evil parts of it are the non-overloading (which we have in C++),
	not binding on placeholder names, and the obtuse way of binding strings.
*/
int SQLiteDriver::bind(sqlite3_stmt *stmt, const char *var_name, int var){
	int parameterIndex = sqlite3_bind_parameter_index(stmt, var_name);
	return sqlite3_bind_int(stmt, parameterIndex, var);
}

int SQLiteDriver::bind(sqlite3_stmt *stmt, const char *var_name, double var){
	int parameterIndex = sqlite3_bind_parameter_index(stmt, var_name);
	return sqlite3_bind_double(stmt, parameterIndex, var);
}

int SQLiteDriver::bind(sqlite3_stmt *stmt, const char *var_name, std::string var){
	int parameterIndex = sqlite3_bind_parameter_index(stmt, var_name);
	//There's some magic here, so here's an explanation
	//Args 1 and 2 are simple enough, 3 is a C string, 4 is the size in bytes of the string,
	// and 5 is a special value saying that the string passed may change in the future
	// (which it does, std::string.c_str() returns an internal representation that changes)
	return sqlite3_bind_text(stmt, parameterIndex, var.c_str(), (var.size() + 1) * sizeof(char), SQLITE_TRANSIENT);
}

//Easy way to check error codes
//true for everything alright, false for bad things
bool SQLiteDriver::checkError(int status){
	if(status == SQLITE_OK || status == SQLITE_DONE){
		return true;
	}else{
		//Get the error message and log it
		fprintf(stderr, "%s", sqlite3_errmsg(db));
		return false;
	}
}

