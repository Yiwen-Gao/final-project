CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

.PHONY: default
default: getcert

getcert: getcert.o

getcert.o: getcert.cpp

client: client.o

client.o: client.c

.PHONY: clean
clean:
	rm -f *.o a.out core client getcert
