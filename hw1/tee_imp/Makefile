CC:=gcc

dep: tee.c
	$(CC) -Wall -Wextra -c tee.c
all: dep
	$(CC) -o tee tee.o
clean:
	-rm -rf *.o
