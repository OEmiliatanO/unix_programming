CC:=gcc

all: dep
	$(CC) -o server server.o
	$(CC) -o client client.o
dep: server.c client.c
	$(CC) -Wall -Wextra -c server.c
	$(CC) -Wall -Wextra -c client.c
clean:
	-rm -rf *.o

