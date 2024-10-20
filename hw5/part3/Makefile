CC:=gcc
FLAG:=-Wall -Wextra

dep: convert.c lookup1.c main.c
	$(CC) $(FLAG) -c convert.c
	$(CC) $(FLAG) -c lookup1.c
	$(CC) $(FLAG) -c main.c
all: dep
	$(CC) -o convert convert.o
	$(CC) -o file_lookup main.o lookup1.o
clean:
	-rm -rf *.o
