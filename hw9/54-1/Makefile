CC:=gcc

all: dep
	$(CC) -o writer writer.o
	$(CC) -o reader reader.o
dep: writer.c reader.c
	$(CC) -Wall -Wextra -c writer.c
	$(CC) -Wall -Wextra -c reader.c
clean:
	-rm -rf *.o

