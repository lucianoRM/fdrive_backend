# Copyright (c) 2014 Cesanta Software
# All rights reserved

PROG = fdrive
CFLAGS = -W -Wall -I../.. -lpthread -g $(CFLAGS_EXTRA) -Iinclude/ -std=c++11 -lz -lrt
CC = g++
SOURCES = $(PROG).cpp mongoose.c librocksdb.a

$(PROG): $(SOURCES)
	$(CC) -o $(PROG) $(SOURCES) $(CFLAGS)

clean:
	rm -rf $(PROG) *.exe *.dSYM *.obj *.exp .*o *.lib
