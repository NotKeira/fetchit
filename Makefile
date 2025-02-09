CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
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
	@./install/$(shell uname | tr '[:upper:]' '[:lower:]').sh
