#ifndef USER_H
#define USER_H

#include <string>
#include <time.h>
#include "SQLDriver.h"

class User{
public:
	User();
	User(SQLDriver *d);
	User(std::string n, int i, time_t t, SQLDriver *d);
	~User();
	
	std::string getName();
	void setName(std::string n);
	int getID();
	void setID(int i);
	
	time_t getStamp();
	SQLDriver* getDriver();
	friend class SQLiteDriver;
private:
	std::string name;
	int id;
	time_t timestamp;
	std::string comment;
	SQLDriver *driver;
	
};


#endif
