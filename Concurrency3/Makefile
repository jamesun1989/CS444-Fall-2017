CC=gcc
PROGS = problem1 problem2

all: problem1 problem2

problem2: problem2.o
	gcc problem2.c -o problem2 -lpthread

problem1:
	gcc problem1.c -o problem1 -lpthread
clean:
	rm -f $(PROGS) *.o *~ \#*