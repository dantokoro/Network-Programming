CC = gcc
all: client.o server.o rsa.c user.c
	$(CC) -o client client.c rsa.c user.c -lm
	$(CC) -o server server.c user.c
clean:
	rm *.o client server
