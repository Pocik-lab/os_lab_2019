CC=gcc
CFLAGS=-I.

all: utils.o find_min_max.o parallel_min_max.o sequential_min_max.o mainexec.o
	$(CC) -o main_min_max find_min_max.o utils.o sequential_min_max.o $(CFLAGS)
	$(CC) -o mainparal parallel_min_max.o utils.o find_min_max.o $(CFLAGS)
	$(CC) -o mainexec mainexec.o $(CFLAGS)

mainexec: mainexec.o
	$(CC) -o mainexec mainexec.o $(CFLAGS)

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

objects : utils.c find_min_max.c sequential_min_max.c parallel_min_max.c mainexec.c
	$(CC) -c utils.c find_min_max.c sequential_min_max.c parallel_min_max.c mainexec.c

clean :
	rm utils.o find_min_max.o sequential_min_max.o mainparal main_min_max parallel_min_max.o mainexec mainexec.o
