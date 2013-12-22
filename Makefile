CC=clang
CFLAGS=-std=gnu99 -Wall -g -ggdb -lreadline
SOURCE= $(wildcard ./*.c ./lib/*)
INCLUDE=-I ./include
OUT=-o
EXECUTABLE=tsh

all:
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) $(OUT) $(EXECUTABLE)

install:
	cp $(EXECUTABLE) /usr/bin/
	#Add to /etc/shells
	cp tsh-man /usr/share/man/man1/$(EXECUTABLE).1
	gzip -f /usr/share/man/man1/$(EXECUTABLE).1

uninstall:
	rm /usr/bin/$(EXECUTABLE)
	#Remove from /etc/shells
	rm /usr/share/man/man1/$(EXECUTABLE).1.gz
