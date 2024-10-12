/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

void run_command(char **myArgv) {
    if (myArgv == NULL) return;

    pid_t pid;
    int stat;

    /* Create a new child process.
     * Fill in code.
	 */
    int not_wait = is_background(myArgv);
    pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork error.");
            exit(errno);

        /* Parent. */
        default :
            /* Wait for child to terminate.
             * Fill in code.
			 */
            if (not_wait)
                fprintf(stdout,"[%d]\n", pid);
            else
                waitpid(pid, &stat, WUNTRACED | WCONTINUED);

            /* Optional: display exit status.  (See wstat(5).)
             * Fill in code.
			 */
            // no, thanks

            return;

        /* Child. */
        case 0 :
            /* Run command in child process.
             * Fill in code.
			 */
            execvp(myArgv[0], myArgv);

            /* Handle error return from exec */
            perror("command error.");
			exit(errno);
    }
}
