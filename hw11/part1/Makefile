CC:=gcc

all: dep
	$(CC) -o usock_client main.o lookup7.o
	$(CC) -o usock_server usock_server.o lookup5.o
dep: dict.h lookup5.c lookup7.c usock_server.c main.c
	$(CC) -Wall -Wextra -c main.c
	$(CC) -Wall -Wextra -c usock_server.c
	$(CC) -Wall -Wextra -c lookup5.c
	$(CC) -Wall -Wextra -c lookup7.c
clean:
	-rm -rf *.o
