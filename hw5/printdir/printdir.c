#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main() {

	char *path = NULL;
	path = getcwd(NULL, 0);
	if (path == NULL) {
		fprintf(stderr, "Cannnot get the current working directory.");
		exit(errno);
	}
	puts(path);
	free(path);

    return 0;
}

