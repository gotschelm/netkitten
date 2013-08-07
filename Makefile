CC=gcc
CFLAGS=-O2

netkitten:	netkitten.c
	$(CC) $(CFLAGS) -o bin/$@ $^

debug:	netkitten.c
	$(CC) -g -o bin/$@ $^
