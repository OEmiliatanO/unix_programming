CC:=gcc
FLAG:=-Wall -Wextra

dep: main.c
	$(CC) $(FLAG) -c main.c
all: dep
	$(CC) -o main.elf main.o
clean:
	-rm -rf *.o
