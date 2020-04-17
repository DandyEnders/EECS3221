# makefile for scheduling program
#
# make schedule - for FCFS / scan / cscan scheduling

CC=gcc
CFLAGS=-Wall

clean:
	rm -rf *.o
	rm -rf schedule

schedule: driver.o schedule.o
	$(CC) $(CFLAGS) -o schedule driver.o schedule.o

schedule.o: schedule.c
	$(CC) $(CFLAGS) -c schedule.c

driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c