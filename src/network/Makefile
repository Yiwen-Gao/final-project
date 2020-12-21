CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)
DEPS = conn.h

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

server: clean conn.o server-conn.o server.o 
	$(CC) $(LDFLAGS) conn.o server-conn.o server.o $(LDLIBS) -o server 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o server