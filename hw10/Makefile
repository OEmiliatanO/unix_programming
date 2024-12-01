CC:=gcc

all: dep
	$(CC) -o main main.o
dep: main.c
	$(CC) -Wall -Wextra -c main.c
clean:
	-rm -rf *.o

