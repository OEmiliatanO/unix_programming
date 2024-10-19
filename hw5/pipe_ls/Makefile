CC:=gcc
FLAG:=-Wall -Wextra

dep: pipe_ls.c
	$(CC) $(FLAG) -c pipe_ls.c
all: dep
	$(CC) -o pipe_ls pipe_ls.o
clean:
	-rm -rf *.o
