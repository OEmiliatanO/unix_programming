/*
 *  pipe_present.c :  check for |
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

/*
 * Return index offset into argv of where "|" is,
 * -1 if in an illegal position (first or last index in the array),
 * or 0 if not present.
 */
int pipe_present(char ** myCurrentArgv) {
	int index = 0;

  	/* Search through myCurrentArgv for a match on "|". */
    for (; myCurrentArgv[index]; ++index) {
        if (strcmp(myCurrentArgv[index], "|") == 0)
            break;
	}
	
  	if (index == 0 || (myCurrentArgv[index] && myCurrentArgv[index+1] == NULL)) {
    	return -1;

  	} else if (myCurrentArgv[index] == NULL) {
    	return 0;

  	} else {
    	/* In the middle. */
    	return index;
  	}
}
