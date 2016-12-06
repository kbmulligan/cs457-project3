CC=g++
CFLAGS=-c -Wall -Werror -std=c++11
LDFLAGS=
SOURCES=manager.cpp router.cpp core.cpp project3.cpp
OBJECTS=manager.o router.o core.o
EXECUTABLES=manager

all: $(SOURCES) $(EXECUTABLES) 

manager: manager.o router.o core.o project3.o
	$(CC) $(LDFLAGS) manager.o router.o core.o project3.o -o manager

core.o: core.cpp
	$(CC) $(CFLAGS) core.cpp

project3.o: core.cpp project3.cpp
	$(CC) $(CFLAGS) core.cpp project3.cpp

manager.o: manager.cpp core.cpp project3.cpp
	$(CC) $(CFLAGS) manager.cpp core.cpp project3.cpp

router.o: router.cpp core.cpp project3.cpp
	$(CC) $(CFLAGS) router.cpp core.cpp project3.cpp

clean:
	rm *.o
