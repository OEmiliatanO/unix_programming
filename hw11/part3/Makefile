CC:=gcc

all: dep
	$(CC) -o udp_client main.o lookup9.o
	$(CC) -o udp_server udp_server.o lookup5.o
dep: dict.h lookup5.c lookup9.c udp_server.c main.c
	$(CC) -Wall -Wextra -c main.c
	$(CC) -Wall -Wextra -c udp_server.c
	$(CC) -Wall -Wextra -c lookup5.c
	$(CC) -Wall -Wextra -c lookup9.c
clean:
	-rm -rf *.o
