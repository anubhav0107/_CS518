CC = gcc
CFLAGS = -g -c
AR = ar -rc
RANLIB = ranlib

all: memgrind.o umalloc.a
	$(CC) memgrind.o umalloc.o -o test

umalloc.a: umalloc.o
	$(AR) libumalloc.a umalloc.o
	$(RANLIB) libumalloc.a

memgrind.o:
	$(CC) $(CFLAGS) memgrind.c -L../ -lumalloc

umalloc.o: umalloc.h
	$(CC) $(CFLAGS) umalloc.c

clean:
	rm *.o *.a test