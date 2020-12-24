CC = g++
CXX = g++

INCLUDES = -I../
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

# getcert: conn.o client-conn.o utils.o reqs.o resps.o getcert.o
# 	$(CC) $(LDFLAGS) conn.o client-conn.o utils.o reqs.o resps.o getcert.o $(LDLIBS) -o getcert 

# changepw: conn.o client-conn.o utils.o reqs.o resps.o changepw.o
# 	$(CC) $(LDFLAGS) conn.o client-conn.o utils.o reqs.o resps.o changepw.o $(LDLIBS) -o changepw 

sendmsg: conn.o client-conn.o utils.o reqs.o resps.o sendmsg.o
	$(CC) $(LDFLAGS) conn.o client-conn.o utils.o reqs.o resps.o sendmsg.o $(LDLIBS) -o sendmsg 

recvmsg: conn.o client-conn.o utils.o reqs.o resps.o recvmsg.o
	$(CC) $(LDFLAGS) reqs.o resps.o conn.o client-conn.o utils.o recvmsg.o $(LDLIBS) -o recvmsg

conn.o: ../conn.cpp
	$(CC) $(CFLAGS) -c ../conn.cpp

client-conn.o: ../client-conn.cpp
	$(CC) $(CFLAGS) -c ../client-conn.cpp

reqs.o: ../reqs.cpp
	$(CC) $(CFLAGS) -c ../reqs.cpp

resps.o: ../resps.cpp
	$(CC) $(CFLAGS) -c ../resps.cpp

utils.o: ../utils.o
	$(CC) $(CFLAGS) -c ../utils.cpp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o a.out getcert changepw sendmsg recvmsg

