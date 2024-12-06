CC:=gcc

all: dep
	$(CC) -o isock_client main.o lookup8.o
	$(CC) -o isock_server isock_server.o lookup5.o
dep: dict.h lookup5.c lookup8.c isock_server.c main.c
	$(CC) -Wall -Wextra -c main.c
	$(CC) -Wall -Wextra -c isock_server.c
	$(CC) -Wall -Wextra -c lookup5.c
	$(CC) -Wall -Wextra -c lookup8.c
clean:
	-rm -rf *.o
