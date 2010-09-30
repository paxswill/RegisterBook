# get uname for some simple OS detection
UNAME :=$(shell sh -c 'uname -s')

# If we're on Windows, assume we're on my flash drive and we need
# make SQLite
ifeq ($(UNAME),MINGW32_NT-5.1)
	MINGW_CFLAGS = '-I../sqlite/'
	MINGW_LDFLAGS = '-L../sqlite/'
else
	MINGW_CFLAGS =
	MINGW_LDFLAGS = 
endif

# OS X compilation flags
ifeq ($(UNAME),Darwin)
	OSX_CFLAGS = 
	OSX_LDFLAGS =
else
	OSX_CFLAGS = 
	OSX_LDFLAGS =
endif

SYS_CFLAGS = $(MINGW_CFLAGS) $(OSX_CFLAGS)
SYS_LDFLAGS = $(MINGW_LDFLAGS) $(OSX_LDFLAGS)

all: sqlite

sqlite:
ifeq ($(UNAME),MINGW32_NT-5.1)
	$(MAKE) ../sqlite/Makefile
endif

