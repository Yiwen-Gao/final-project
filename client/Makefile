CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

.PHONY: default
default: getcert

clienttools: StringBIO.o clienttools.o

StringBIO.o: StringBIO.cpp StringBIO.h

clienttools.o: clienttools.cpp StringBIO.h clienttools.h

getcert: getcert.o clienttools.o

getcert.o: getcert.cpp clienttools.h

client: client.o

client.o: client.c

.PHONY: clean
clean:
	rm -f *.o a.out core client getcert
