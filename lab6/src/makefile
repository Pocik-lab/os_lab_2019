CC=gcc
CFLAG=-I.

all : server client

server : server.o
	$(CC) server.o -lpthread -L. -lmultmodulo -o ./server $(CFLAG)

client : client.o
	$(CC) client.o -lpthread -L. -lmultmodulo -o ./client $(CFLAG)

server.o :
	$(CC) -c server.c $(CFLAG)

client.o : 
	$(CC) -c client.c $(CFLAG)

libmultmodulo:
	$(CC) -c multmodulo.c $(CFLAG)
	ar rc libmultmodulo.a multmodulo.o
	ranlib libmultmodulo.a

clean :
	rm server client
