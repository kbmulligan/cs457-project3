CC=g++
CFLAGS=-c -Wall -Werror -std=c++11
LDFLAGS=
SOURCES=manager.cpp router.cpp
OBJECTS=manager.o router.o
EXECUTABLES=manager router

all: $(SOURCES) $(EXECUTABLES) 

manager: manager.o core.o
	$(CC) $(LDFLAGS) manager.o core.o -o manager

router: router.o core.o
	$(CC) $(LDFLAGS) router.o core.o -o router 

core.o: core.cpp
	$(CC) $(CFLAGS) core.cpp

manager.o: manager.cpp 
	$(CC) $(CFLAGS) manager.cpp 

router.o: router.cpp core.cpp
	$(CC) $(CFLAGS) router.cpp 

clean:
	rm *.o
