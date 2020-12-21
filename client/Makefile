CC = gcc
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

client: client.o

client.o: client.c

.PHONY: clean
clean:
	rm -f *.o a.out core client
