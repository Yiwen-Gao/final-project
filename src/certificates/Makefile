mail-in: get-cert.o
	g++ -std=c++17 get-cert.o -lstdc++fs -o get-cert

get-cert.o: get-cert.cpp
	g++ -std=c++17 -c get-cert.cpp

.PHONY: clean
clean:
	rm *.o get-cert

.PHONY: install
install: get-cert
	cp get-cert $(TREE)/bin
