#
# Makefile for security final project - Security 1
# Group evilhackerdudes.org
#

CC = gcc
CXX = g++

INCLUDES = -lssl -lcrypto
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g

.PHONY: default
default: client

client: tls-client-starter-code.o

.PHONY: clean
clean:
	rm -f *.o a.out core client

