/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/utsname.h>
#include "shell.h"

/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/
static void bi_builtin(char ** argv);	/* "builtin" command tells whether a command is builtin or not. */
static void bi_cd(char **argv) ;		/* "cd" command. */
static void bi_echo(char **argv);		/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_hostname(char ** argv);	/* "hostname" command. */
static void bi_id(char ** argv);		/* "id" command shows user and group of this process. */
static void bi_pwd(char ** argv);		/* "pwd" command. */
static void bi_quit(char **argv);		/* quit/exit/logout/bye command. */




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
  	char * keyword;					/* When this field is argv[0] ... */
  	void (* do_it)(char **);		/* ... this function is executed. */
} inbuilts[] = {
  	{ "builtin",    bi_builtin },   /* List of (argv[0], function) pairs. */

    /* Fill in code. */
    { "echo",       bi_echo },
    { "quit",       bi_quit },
    { "exit",       bi_quit },
    { "bye",        bi_quit },
    { "logout",     bi_quit },
    { "cd",         bi_cd },
    { "pwd",        bi_pwd },
    { "id",         bi_id },
    { "hostname",   bi_hostname },
    {  NULL,        NULL }          /* NULL terminated. */
};


static void bi_builtin(char ** argv) {
	(void) argv; // to avoid the warning 
    fprintf(stdout, "Builtin command: echo, quit, exit, bye, logout, cd, pwd, id, hostname\n");
}

static void bi_cd(char **argv) {
    if (argv[1] == NULL) {
        fprintf(stderr, "usage: cd [DIR]\n");
        return;
    }
	if (chdir(argv[1]) != 0)
        fprintf(stderr, "Cannot change working directory into %s\n", argv[1]);
}

static void bi_echo(char **argv) {
    if (argv[1] == NULL) {
        fprintf(stderr, "usage: echo [string] [...]\n");
        return;
    }
    fprintf(stdout, "%s", argv[1]);
    for (int i = 2; argv[i]; ++i) {
        fprintf(stdout, " %s", argv[i]);
    }
    fprintf(stdout, "\n");
}

static void bi_hostname(char ** argv) {
	(void) argv; // to avoid the warning 
	char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        fprintf(stderr, "Cannot get the hostname.\n");
        return;
    }
    fprintf(stdout, "%s\n", hostname);
}

static void bi_id(char ** argv) {
	(void) argv; // to avoid the warning 
    uid_t uid = getuid(); // getuid & getgid are always successful
 	gid_t gid = getgid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        fprintf(stderr, "Cannot get the name of current user.\n");
        return;
    }
    struct group *gr = getgrgid(gid);
    if (gr == NULL) {
        fprintf(stderr, "Cannot get the name of current group.\n");
        return;
    }
    fprintf(stdout, "UserID = %d(%s), GroupID = %d(%s)\n", uid, pw->pw_name, gid, gr->gr_name);
}

static void bi_pwd(char ** argv) {
	(void) argv; // to avoid the warning 
    char *path = getcwd(NULL, 0);
    if (path == NULL) {
        fprintf(stderr, "Cannot get the current working directory.\n");
        return;
    }
    fprintf(stdout, "%s\n", path);
    free(path);
}

static void bi_quit(char **argv) {
	(void) argv; // to avoid the warning 
	exit(0);
}


/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; /* close coupling between is_builtin & do_builtin */

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
