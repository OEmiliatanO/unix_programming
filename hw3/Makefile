CC:=gcc
FLAG:=-Wall -Wextra

dep: builtin.c is_background.c parse.c run_command.c shell.c shell.h
	$(CC) $(FLAG) -c builtin.c
	$(CC) $(FLAG) -c is_background.c
	$(CC) $(FLAG) -c parse.c
	$(CC) $(FLAG) -c run_command.c
	$(CC) $(FLAG) -c shell.c
all: dep
	$(CC) -o shell shell.o builtin.o is_background.o parse.o run_command.o
clean:
	-rm -rf *.o
