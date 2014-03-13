CC=clang
CFLAGS= -Wall -Wextra -Werror -pedantic -O2 -pipe -march=native
DEBUG_CFLAGS= -Wall -Wextra -Werror -pedantic -O0 -g -ggdb -pipe -DSTRUTIL_DEBUG -DVECTOR_DEBUG -DHASH_DEBUG
SOURCE= $(wildcard ./src/* ./lib/data-structs/* ./lib/strutil/*)
INCLUDE=-I ./include
LFLAGS= -lreadline
OUT=-o
EXECUTABLE=tsh

# Default values for user-supplied compile time directives:
DEBUG_MSG=

# Enable debugging messages outside of the 'debug' target:
ifeq ($(DEBUG_MSG),y)
	CFLAGS += -DDEBUG_MSG_ENABLE
endif

.PHONY: all
all:
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) $(OUT) $(EXECUTABLE) $(LFLAGS)

.PHONY: debug
debug:
	$(CC) $(DEBUG_CFLAGS) $(INCLUDE) $(SOURCE) $(OUT) $(EXECUTABLE) $(LFLAGS)

.PHONY: install
install:
	mv ./$(EXECUTABLE) /usr/bin/
	echo "/usr/bin/tsh" >> /etc/shells
	cp tsh-man /usr/share/man/man1/$(EXECUTABLE).1
	gzip -f /usr/share/man/man1/$(EXECUTABLE).1

.PHONY: uninstall
uninstall:
	rm /usr/bin/$(EXECUTABLE)
	#Remove from /etc/shells
	rm /usr/share/man/man1/$(EXECUTABLE).1.gz

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(EXECUTABLE)
