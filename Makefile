CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -O3 -march=native -flto -DNDEBUG
SRC = $(wildcard src/sys/*.c) src/main.c
OUT = fetchit

# Auto-detect install location based on permissions
ifeq ($(shell id -u), 0)
    PREFIX = /usr
else
    PREFIX = /usr/local
endif

DESTDIR =

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)
	strip $(OUT)

clean:
	rm -f $(OUT)

install: $(OUT)
	@echo "Installing to $(PREFIX)/bin"
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(OUT) $(DESTDIR)$(PREFIX)/bin/

.PHONY: all clean install