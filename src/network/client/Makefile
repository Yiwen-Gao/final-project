CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

getcert: getcert.o

getcert.o: getcert.cpp

.PHONY: clean
clean:
	rm -f *.o a.out core client getcert
