CC:=gcc
FLAG:=-Wall -Wextra

dep: main.c
	$(CC) $(FLAG) -c main.c
all: dep
	$(CC) -o main main.o
clean:
	-rm -rf *.o
