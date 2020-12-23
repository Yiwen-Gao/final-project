CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

.PHONY: default
default: getcert changepw sendmsg

sendmsg: sendmsg.o

sendmsg.o: sendmsg.cpp

getcert: getcert.o

getcert.o: getcert.cpp

changepw: changepw.o

changepw.o: changepw.cpp

.PHONY: clean
clean:
	rm -f *.o a.out core client getcert changepw
