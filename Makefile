# get uname for some simple OS detection
UNAME :=$(shell sh -c 'uname -s')

# If we're on Windows, assume we're on my flash drive and we need
# make SQLite
ifeq ($(UNAME),MINGW32_NT-5.1)
	MINGW_CFLAGS =
	MINGW_LDFLAGS =
else
	MINGW_CFLAGS =
	MINGW_LDFLAGS = 
endif

# OS X compilation flags
ifeq ($(UNAME),Darwin)
	OSX_CFLAGS = 
	OSX_LDFLAGS =
	# Prefer clang on OS X
	CC=clang
	CXX=clang
else
	OSX_CFLAGS = 
	OSX_LDFLAGS =
	CC=gcc
	CXX=g++
endif

SYS_CFLAGS = $(MINGW_CFLAGS) $(OSX_CFLAGS)
SYS_LDFLAGS = $(MINGW_LDFLAGS) $(OSX_LDFLAGS)

#Compiler flags
SELF_CFLAGS = -g -O0 -Wall $(CFLAGS)
SELF_LDFLAGS =

all: sqlite3 transaction sqlite testing
	#Link everything up
	$(CXX) $(SELF_LDFLAGS) $(SYS_LDFLAGS) ./bin/test.o ./bin/sqlite3.o ./bin/Transaction.o ./bin/SQLiteDriver.o -o ./bin/RegisterBook

clean:
	rm -rf ./bin/*

testing: transaction sqlite
	$(CXX) $(SELF_CFLAGS) $(SYS_CFLAGS) -c -I./src/ ./src/test.cpp -o ./bin/test.o

sqlite3:
	$(CC) $(SELF_CFLAGS) $(SYS_CFLAGS) -c -I./sqlite3/ ./sqlite3/sqlite3.c -o ./bin/sqlite3.o

transaction:
	$(CXX) $(SELF_CFLAGS) $(SYS_CFLAGS) -c -I./src/ ./src/Transaction.cpp -o ./bin/Transaction.o

sqlite: sqlite3 transaction
	$(CXX) $(SELF_CFLAGS) $(SYS_CFLAGS) -c -I./src/ -I./sqlite3/ ./src/SQLiteDriver.cpp -o ./bin/SQLiteDriver.o

