/*
 * shell.c  : test harness for parse routine
 */


#define LONGLINE 255

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

// deal with the zombie child process
void sigchld_handler(int signum) {
    (void) signum; // prevent the unused warning
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        fprintf(stdout, "PID %d finished.\n", pid);
    }
}

int main() {
    // set signal handler
    signal(SIGCHLD, sigchld_handler);

    char line[LONGLINE];
    char **myArgv;

    fputs("myshell -> ", stdout);
    while (fgets(line, LONGLINE, stdin)) {

        /* Create argv array based on commandline. */
        if ((myArgv = parse(line))) {
            for (int i = 0; myArgv[i] != NULL; ++i) fprintf(stdout, "[%d] : %s\n", i, myArgv[i]);
           
            if (is_builtin(myArgv[0])) { /* If command is recognized as a builtin, do it. */
                do_builtin(myArgv);         
            } else {					/* Non-builtin command. */
	            run_command(myArgv);
            }

            free_argv(myArgv);			/* Free argv array. */
        }

        fputs("myshell -> ", stdout);
    }
    exit(0);
}
