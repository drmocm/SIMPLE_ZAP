CFLAGS =  -g  -Wno-unused -Wall -Wno-format -O2 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE
LIBS = 
OBJ = dvb.o simple_zap.o
HEADER = dvb.h
INCS = -I.

CC = gcc
CPP = g++

.PHONY: clean 

TARGETS = simple_zap

all: $(TARGETS)

simple_zap: $(OBJ) dvb.h 
	$(CC) $(CFLAGS) -o simple_zap $(OBJ) $(LIBS)

dvb.o: $(HEADER) dvb.c
	$(CC) -c $(CFLAGS) $(INCS) $(DEFINES) dvb.c

simple_zap.o: $(HEADER) simple_zap.c
	$(CC) -c $(CFLAGS) $(INCS) $(DEFINES) simple_zap.c


clean:
	for f in $(TARGETS) *.o .depend *~ ; do \
		if [ -e "$$f" ]; then \
			rm "$$f" || exit 1; \
		fi \
	done

