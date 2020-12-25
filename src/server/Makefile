CC = g++
CXX = g++

INCLUDES = -I../network
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall -std=c++17 $(INCLUDES)
DEPS = conn.h priv.h

LDFLAGS = -g
LDLIBS = -lssl -lcrypto -lstdc++fs

# server: clean conn.o server-conn.o server.o
# 	$(CC) $(LDFLAGS) conn.o server-conn.o server.o $(LDLIBS) -o server

server: clean conn.o server-conn.o reqs.o resps.o utils.o server.o
	$(CC) $(LDFLAGS) conn.o server-conn.o reqs.o resps.o utils.o server.o $(LDLIBS) -o server  

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

conn.o: ../network/conn.cpp
	$(CC) $(CFLAGS) -c ../network/conn.cpp

server-conn.o: ../network/server-conn.cpp
	$(CC) $(CFLAGS) -c ../network/server-conn.cpp

reqs.o: ../network/reqs.cpp
	$(CC) $(CFLAGS) -c ../network/reqs.cpp

resps.o: ../network/resps.cpp
	$(CC) $(CFLAGS) -c ../network/resps.cpp

utils.o: ../network/utils.o
	$(CC) $(CFLAGS) -c ../network/utils.cpp

.PHONY: clean
clean:
	rm -f *.o server
