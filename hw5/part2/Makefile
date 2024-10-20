CC:=gcc
FLAG:=-Wall -Wextra

dep: builtin.c is_background.c parse.c pipe_command.c pipe_present.c redirect_in.c redirect_out.c run_command.c shell.c shell.h
	$(CC) $(FLAG) -c builtin.c
	$(CC) $(FLAG) -c is_background.c
	$(CC) $(FLAG) -c parse.c
	$(CC) $(FLAG) -c pipe_command.c
	$(CC) $(FLAG) -c pipe_present.c
	$(CC) $(FLAG) -c redirect_in.c
	$(CC) $(FLAG) -c redirect_out.c
	$(CC) $(FLAG) -c run_command.c
	$(CC) $(FLAG) -c shell.c
all: dep
	$(CC) -o shell shell.o builtin.o is_background.o parse.o pipe_command.o pipe_present.o redirect_in.o redirect_out.o run_command.o
clean:
	-rm -rf *.o
