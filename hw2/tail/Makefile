CC:=gcc

dep: tail.c
	$(CC) -Wall -Wextra -c tail.c
all: dep
	$(CC) -o tail tail.o
clean:
	-rm -rf *.o
