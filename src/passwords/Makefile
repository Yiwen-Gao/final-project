CC = g++
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)
DEPS = db-manager.h

LDFLAGS = -g
LDLIBS = -lcrypt

.PHONY: default
default: crypt-pw verify-pw change-pw

crypt-pw: db-manager.o crypt-pw.o
	$(CC) $(LDFLAGS) db-manager.o crypt-pw.o $(LDLIBS) -o crypt-pw 
	cp crypt-pw ../../server/passwords

verify-pw: db-manager.o verify-pw.o
	$(CC) $(LDFLAGS) db-manager.o verify-pw.o $(LDLIBS) -o verify-pw 
	cp verify-pw ../../server/passwords

change-pw: db-manager.o change-pw.o
	$(CC) $(LDFLAGS) db-manager.o change-pw.o $(LDLIBS) -o change-pw 
	cp change-pw ../../server/passwords

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o crypt-pw verify-pw change-pw
