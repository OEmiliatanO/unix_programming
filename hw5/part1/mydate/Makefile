CC:=gcc
FLAG:=-Wall -Wextra

dep: mydate.c
	$(CC) $(FLAG) -c mydate.c
all: dep
	$(CC) -o mydate mydate.o
clean:
	-rm -rf *.o
