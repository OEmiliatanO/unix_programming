CC:=gcc

all: dep
	$(CC) -o cp cp.o
dep: cp.c
	$(CC) -Wall -Wextra -c cp.c
clean:
	-rm -rf *.o

