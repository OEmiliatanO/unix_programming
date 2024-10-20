CC:=gcc
FLAG:=-Wall -Wextra

dep: hostinfo.c
	$(CC) $(FLAG) -c hostinfo.c
all: dep
	$(CC) -o hostinfo hostinfo.o
clean:
	-rm -rf *.o
