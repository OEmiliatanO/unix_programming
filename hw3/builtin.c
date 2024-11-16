/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"




/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/

/* "echo" command.  ~~Does not print final <CR> if "-n" encountered.~~ */
/* -n N print the N-th argument */
static void bi_echo(char **argv) {
    int flag = 0;
    int cnt = 0;
    int N = -1; // 1-base
    int exppos = -1; // 1-base
    for (cnt = 0; argv[cnt] != NULL; ++cnt)
        if (strcmp(argv[cnt], "-n") == 0) {
            if (argv[cnt+1] == NULL || (flag = sscanf(argv[cnt+1], "%d", &N)) <= 0) flag = 0;
            else { flag = 1; exppos = cnt; }
        }

    if (flag) {
        int realN = N < exppos ? N : N+2;
        if (realN >= cnt || realN <= 0) {
            perror("N is out of range.");
            return;
        }
        int write_size = strlen(argv[realN]) * sizeof(char);
        if (fprintf(stdout, "%s ", argv[realN]) != (write_size+1)) {
            perror("Cannot write to stdout.");
            return;
        }
    } else {
        for (int i = 1; argv[i] != NULL; ++i) {
            int write_size = strlen(argv[i]) * sizeof(char);
            if (fprintf(stdout, "%s ", argv[i]) != (write_size+1)) {
                perror("Cannot write to stdout.");
                return;
            }
        }
    }
    fprintf(stdout, "\n");
}

static void bi_exit(char **argv) {
    (void)argv;
    exit(0);
}




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
	char * keyword;				/* When this field is argv[0] ... */
	void (* do_it)(char **);	/* ... this function is executed. */
} inbuilts[] = {

	/* Fill in code. */

	{ "echo", bi_echo },		/* When "echo" is typed, bi_echo() executes.  */
    { "exit", bi_exit }, 
    { "quit", bi_exit }, 
    { "bye",  bi_exit }, 
    { "logout", bi_exit }, 
	{ NULL, NULL }				/* NULL terminated. */
};




/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; 		/* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {
  	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
			this = tableCommand;
			return 1;
		}
  	return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
int do_builtin(char **argv) {
  	this->do_it(argv);
    return 1;
}
