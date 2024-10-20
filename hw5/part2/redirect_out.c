/*
 * redirect_out.c   :   check for >
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for ">" in myArgv, then redirect output to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_out(char ** myArgv) {
	int i = 0;
  	int fd;

  	/* search forward for >
  	 * Fill in code. */
    for (; myArgv[i]; ++i) {
        if (strcmp(myArgv[i], ">") == 0) 
			break;
	}

  	if (myArgv[i]) {	/* found ">" in vector. */
        // 1. open file
        fd = open(myArgv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 00666);
		if (fd == -1) {
			fprintf(stderr, "Cannot open the file, %s\n", myArgv[i+1]);
		}

        // 2. redirect
        if (dup2(fd, STD_OUTPUT) == -1) { // stdout <= fd
            fprintf(stderr, "Cannot redirect stdin to fd.\n");
            return -1;
        }

        // 3. cleanup
        close(fd);

        // 4. remove the ">" and filename
        while (myArgv[i]) {
            myArgv[i] = myArgv[i+2];
            ++i;
        }
  	}
  	return 0;
}
