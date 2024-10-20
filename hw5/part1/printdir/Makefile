CC:=gcc
FLAG:=-Wall -Wextra

dep: printdir.c
	$(CC) $(FLAG) -c printdir.c
all: dep
	$(CC) -o printdir printdir.o
clean:
	-rm -rf *.o
