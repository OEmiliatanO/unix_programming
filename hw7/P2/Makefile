CC:=gcc

all: dep
	$(CC) -o main main.o mysig.o
dep: main.c mysig.c
	$(CC) -Wall -Wextra -c main.c
	$(CC) -Wall -Wextra -c mysig.c
clean:
	-rm -rf *.o
