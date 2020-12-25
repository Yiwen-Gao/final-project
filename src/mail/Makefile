get-msg: get-msg.o mail_utils.o
	g++ -std=c++17 get-msg.o mail_utils.o -lstdc++fs -o get-msg

get-msg.o: get-msg.cpp
	g++ -std=c++17 -c get-msg.cpp

mail_utils.o: mail_utils.cpp
	g++ -std=c++17 -c mail_utils.cpp

.PHONY: clean
clean:
	rm -rf *.o get-msg
