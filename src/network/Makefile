CC = g++
CXX = g++

INCLUDES =
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

.PHONY: clean
clean:
	rm -f *.o server
