CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
SRC = $(wildcard src/sys/*.c) src/main.c
OUT = fetchit

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)

install: all
	@echo "Installing FetchIt..."
	@./install/$(shell uname | tr '[:upper:]' '[:lower:]').sh
