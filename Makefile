# Makefile -- This file is part of the ccat project.
CC = gcc
CFLAGS = -Wall -D_GNU_SOURCE
LDFLAGS = -pie

PROG = ccat
SRC = $(shell echo *.c)
DEPS = $(shell echo *.h)
OBJ = $(SRC:.c=.o)
CFGS = $(shell echo configs/*.cfg)
CFG_DIR = /usr/local/etc/$(PROG)

all: $(PROG)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROG): $(OBJ)
	$(CC) -o $@ $+ $(LDFLAGS)

install: all
	mkdir -p $(CFG_DIR)
	install -m 644 -g root -o root $(CFGS) $(CFG_DIR)/
	install -m 755 -g root -o root $(PROG) /usr/local/bin/

uninstall:
	rm -fr $(CFG_DIR)
	rm -f /usr/local/bin/$(PROG)

clean:
	rm -f $(PROG) $(OBJ)

.PHONY: all install uninstall clean
