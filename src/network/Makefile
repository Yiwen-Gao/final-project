CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)
DEPS = conn.h priv.h

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

server: clean conn.o server-conn.o server.o
	$(CC) $(LDFLAGS) conn.o server-conn.o server.o $(LDLIBS) -o server 

changepw: conn.o client-conn.o changepw.o
	$(CC) $(LDFLAGS) conn.o server-conn.o server.o $(LDLIBS) -o changepw 
	cp ./sendmsg ../../client

sendmsg: conn.o client-conn.o sendmsg.o
	$(CC) $(LDFLAGS) conn.o server-conn.o server.o $(LDLIBS) -o sendmsg 
	cp ./sendmsg ../../client

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o server
