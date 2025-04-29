CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g -pthread
SRC = $(wildcard src/sys/*.c) src/main.c
OUT = fetchit

all: set_permissions
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

set_permissions:
	chmod -R u+rwx src/sys

clean:
	rm -f $(OUT)

install: all
	@echo "Installing FetchIt..."
	@cp $(OUT) /usr/local/bin/fetchit
	@chmod +x /usr/local/bin/fetchit
	@clear
	@echo "FetchIt installed to /usr/local/bin. You can now run 'fetchit' from anywhere."
