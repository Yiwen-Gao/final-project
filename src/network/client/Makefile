CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)
DEPS = conn.h 

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

.PHONY: default
default: getcert changepw recvmsg

recvmsg: recvmsg.o

recvmsg.o: recvmsg.cpp

getcert: getcert.o

changepw: changepw.o

sendmsg: conn.o client-conn.o sendmsg.o
	$(CC) $(LDFLAGS) conn.o server-conn.o server.o $(LDLIBS) -o sendmsg 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o a.out core client getcert changepw
