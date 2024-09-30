#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

void usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [FILE...]\n", program_name);
    exit(EXIT_FAILURE);
}

int pick_flags(const char *argu) {
	if (strcmp(argu, "-a") == 0 || strcmp(argu, "--append") == 0)
		return 1;
	return 0;
}

int main(int argc, char *argv[]) {
    int files[argc - 1];
    memset(files, -1, sizeof(files));
    int filemode = O_WRONLY | O_CREAT;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

	int flags = 0;
    for (int i = 1; i < argc; ++i)
		flags |= pick_flags(argv[i]); // Look for flags, use bitmap to record

    if (flags != 0) filemode |= O_APPEND;
    else            filemode |= O_TRUNC;

    if (argc < 2) {
        usage(argv[0]);
    }

    for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-')
			continue;
        files[i - 1] = open(argv[i], filemode, 0644);
        if (files[i - 1] == -1) { // File errors
			sprintf(buffer, "Cannot open the file, %s", argv[i]);
            perror(buffer);
            exit(EXIT_FAILURE);
        }
    }

	// Read from the stdin
    while ((bytes_read = read(fileno(stdin), (void *)buffer, BUFFER_SIZE)) > 0) {
		// Output to the stdout
        if (write(fileno(stdout), buffer, bytes_read) == -1) {
            perror("Cannot write text to stdout.");
            exit(EXIT_FAILURE);
        }
        
		// Also output to the files
        for (int i = 0; i < argc - 1; i++) {
			if (files[i] == -1) continue;
            if (write(files[i], buffer, bytes_read) == -1) {
                sprintf(buffer, "Cannot write to %s", argv[i+1]);
                perror(buffer);
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < argc - 1; i++) {
		if (files[i] == -1) continue;
        close(files[i]);
    }

    return 0;
}

