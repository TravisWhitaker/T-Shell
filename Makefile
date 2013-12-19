CC=clang
CFLAGS=-std=gnu99 -Wall -g -ggdb -lreadline
SOURCE= $(wildcard ./*.c ../lib/*)
INCLUDE=-I ~/Code/C/Projects/include
OUT=-o
EXECUTABLE=tsh

all:
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) $(OUT) $(EXECUTABLE)

install:
	cp $(EXECUTABLE) /usr/bin/
	cp tsh-man /usr/share/man/man1/$(EXECUTABLE).1
	gzip /usr/share/man/man1/$(EXECUTABLE).1

uninstall:
	rm /usr/bin/$(EXECUTABLE)
	rm /usr/share/man/man1/$(EXECUTABLE).1.gz
