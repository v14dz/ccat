# Makefile -- This file is part of the ccat project.
CC = gcc
CFLAGS = -Wall -D_GNU_SOURCE
INSTALL = install -m 755
# INSTALL = cp

FILE = ccat
SRC = ccat.c colors.c

$(FILE): $(SRC) $(DEPS)
	$(CC) -o $@ $+ $(CFLAGS)

install:
	$(INSTALL) ccat /usr/local/bin/

clean:
	rm -f $(FILE)
