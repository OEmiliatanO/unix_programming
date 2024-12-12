CC:=gcc

all: dep
	$(CC) -o shm_client main.o lookup6.o
	$(CC) -o shm_server shm_server.o lookup5.o
dep: dict.h lookup5.c lookup6.c shm_server.c main.c
	$(CC) -Wall -Wextra -c main.c
	$(CC) -Wall -Wextra -c shm_server.c
	$(CC) -Wall -Wextra -c lookup5.c
	$(CC) -Wall -Wextra -c lookup6.c
clean:
	-rm -rf *.o
