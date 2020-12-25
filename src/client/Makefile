CC = g++
CXX = g++

INCLUDES = -I../network
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)
DEPS = conn.h

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

.PHONY: default
default: getcert changepw sendmsg recvmsg

getcert: getcert.o

getcert.o: getcert.cpp

changepw: changepw.o

changepw.o: changepw.cpp

sendmsg: conn.o client-conn.o utils.o reqs.o resps.o sendmsg.o
	$(CC) $(LDFLAGS) conn.o client-conn.o utils.o reqs.o resps.o sendmsg.o $(LDLIBS) -o sendmsg 

recvmsg: conn.o client-conn.o utils.o reqs.o resps.o recvmsg.o
	$(CC) $(LDFLAGS) reqs.o resps.o conn.o client-conn.o utils.o recvmsg.o $(LDLIBS) -o recvmsg

conn.o: ../network/conn.cpp
	$(CC) $(CFLAGS) -c ../network/conn.cpp

client-conn.o: ../network/client-conn.cpp
	$(CC) $(CFLAGS) -c ../network/client-conn.cpp

reqs.o: ../network/reqs.cpp
	$(CC) $(CFLAGS) -c ../network/reqs.cpp

resps.o: ../network/resps.cpp
	$(CC) $(CFLAGS) -c ../network/resps.cpp

utils.o: ../network/utils.o
	$(CC) $(CFLAGS) -c ../network/utils.cpp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o getcert changepw sendmsg recvmsg

