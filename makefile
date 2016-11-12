CC=g++
CFLAGS=-c -Wall -Werror
LDFLAGS=
SOURCES=manager.cpp router.cpp
OBJECTS=manager.o router.o
EXECUTABLES=manager router

all: $(SOURCES) $(EXECUTABLES) 

manager: manager.o
	$(CC) $(LDFLAGS) manager.o -o manager

router: router.o
	$(CC) $(LDFLAGS) router.o -o router 

manager.o: manager.cpp
	$(CC) $(CFLAGS) manager.cpp 

router.o: router.cpp
	$(CC) $(CFLAGS) router.cpp 

clean:
	rm *.o
