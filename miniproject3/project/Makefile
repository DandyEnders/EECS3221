# makefile for virtual memeory unit program
#
# make mmu

CC=gcc
CFLAGS=-Wall

clean:
	rm -rf *.o
	rm -rf mmu

mmu: mmu.o
	$(CC) $(CFLAGS) -o mmu mmu.c

mmu.o: mmu.c
	$(CC) $(CFLAGS) -c mmu.c