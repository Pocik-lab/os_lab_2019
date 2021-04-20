CC=gcc
CFLAGS=-I.

mainlock: deadlock.o
	$(CC) deadlock.o -L. -lpthread -o mainlock $(CFLAGS)

deadlock.o: deadlock.c
	$(CC) -c deadlock.c $(CFLAGS)

mainmutex: mutex.o
	$(CC) mutex.o -L. -lpthread -o mainmutex $(CFLAGS)

mutex.o: mutex.c
	$(CC) -c mutex.c $(CFLAGS)

mainfact: factorial_with_mutex.o
	$(CC) factorial_with_mutex.o -L. -lpthread -o mainfact $(CFLAGS)

factorial_with_mutex.o: factorial_with_mutex.c
	$(CC) -c factorial_with_mutex.c $(CFLAGS)

clean:
	rm mainmutex mainlock mainfact mutex.o deadlock.o factorial_with_mutex.o
