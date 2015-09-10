PROG = fdrive
CFLAGS =  -I../.. -lpthread -g $(CFLAGS_EXTRA) -Iinclude/ -std=c++11 -lz -lrt
CC = g++
SOURCES = *.cpp mongoose.c librocksdb.a

$(PROG): $(SOURCES)
	$(CC) -o $(PROG) $(SOURCES) $(CFLAGS)

clean:
	rm -rf $(PROG) *.exe *.dSYM *.obj *.exp .*o *.lib
