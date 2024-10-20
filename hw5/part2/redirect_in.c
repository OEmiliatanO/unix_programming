/*
 * redirect_in.c  :  check for <
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for "<" in myArgv, then redirect input to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_in(char ** myArgv) {
  	int i = 0;
  	int fd;

  	/* search forward for <
  	 *
	 * Fill in code. */
    for (; myArgv[i]; ++i) {
        if (strcmp(myArgv[i], "<") == 0)
            break;
	}

  	if (myArgv[i]) {	/* found "<" in vector. */

        // 1. open file
        fd = open(myArgv[i+1], O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "Cannot open the file, %s\n", myArgv[i+1]);
            return -1;
        }

        // 2. redirect
        if (dup2(fd, STD_INPUT) == -1) { // stdin <= fd
            fprintf(stderr, "Cannot duplicate the file descriptor.\n");
            close(fd);
            return -1;
        }

        // 3. cleanup
        close(fd);

        // 4. remove "<" and the filename
        while(myArgv[i]) {
            myArgv[i] = myArgv[i+2];
            ++i;
        }
  	}
  	return 0;
}
