CC=gcc
CFLAGS=-g -std=gnu99 -Wall -ggdb -lreadline
SOURCE= $(wildcard tsh.c redirection.c ../lib/*)
INCLUDE=-I ~/Code/C/Projects/include
OUT=-o
EXECUTABLE=tsh

all:
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) $(OUT) $(EXECUTABLE)

install:
	cp $(EXECUTABLE) /usr/bin/

un-install:
	rm /usr/bin/$(EXECUTABLE)
