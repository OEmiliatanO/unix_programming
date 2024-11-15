CC:=gcc

all: dep
	$(CC) -o server server.o
	$(CC) -o client client.o
	$(CC) -o misbehaving_client misbehaving_client.o
dep: server.c client.c misbehaving_client.c fifo_seqnum.h
	$(CC) -Wall -Wextra -c server.c
	$(CC) -Wall -Wextra -c client.c
	$(CC) -Wall -Wextra -c misbehaving_client.c
clean:
	-rm -rf *.o

