/*
This is the parent class for the SQL drivers.
Forseeable subclasses are those for SQLite and later, MySQL
Also, an XML export function may be tacked onto this as a child
*/
#ifndef SQL_DRIVER_H
#define SQL_DRIVER_H

#include <sqlite3.h>
#include <string>
#include "Transaction.h"

class SQLDriver{
public:
	virtual void open(std::string name) = 0;
	virtual void close() = 0;
	virtual bool commitTransaction(Transaction *t) = 0;
	virtual Transaction* makeTransaction() = 0;
};

#endif
