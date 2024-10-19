CC:=gcc
FLAG:=-Wall -Wextra

dep: mycat.c
	$(CC) $(FLAG) -c mycat.c
all: dep
	$(CC) -o mycat mycat.o
clean:
	-rm -rf *.o
