/*
 * is_background.c :  check for & at end
 */


#include <stdio.h>
#include <string.h>
#include "shell.h"

int is_background(char ** myArgv) {

  	if (*myArgv == NULL)
    	return 0;
    
    int i = 0;
    while (myArgv[i] != NULL) ++i;
    
    --i;
    if (i > 0 && strcmp(myArgv[i], "&") == 0)
    {
        myArgv[i] = NULL;
        return 1;
    }

    return 0;
}
