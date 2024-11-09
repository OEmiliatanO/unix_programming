CC:=gcc

all: dep
	$(CC) -o sigwaitinfo sigwaitinfo.o
	$(CC) -o sigsuspend sigsuspend.o
dep: sigwaitinfo.c sigsuspend.c
	$(CC) -Wall -Wextra -c sigwaitinfo.c
	$(CC) -Wall -Wextra -c sigsuspend.c
clean:
	-rm -rf *.o
