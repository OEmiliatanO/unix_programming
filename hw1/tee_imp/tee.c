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
    FILE *files[argc - 1];
	char filemode[] = "w";
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

	int flags = 0;
	for (int i = 1; i < argc; ++i)
		flags |= pick_flags(argv[i]); // look for flags, use bitmap to record
	filemode[0] = ((flags & 0x1) ? 'a' : 'w');

    if (argc < 2) {
        usage(argv[0]);
    }

    for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') { 
			files[i - 1] = NULL; 
			continue;
		}
        files[i - 1] = fopen(argv[i], filemode);
        if (files[i - 1] == NULL) { // File errors
			sprintf(buffer, "Cannot open the file, %s", argv[i]);
            perror(buffer);
            exit(EXIT_FAILURE);
        }
    }

	// Read from the stdin
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, stdin)) > 0) {
		// Output to the stdout
        fwrite(buffer, 1, bytes_read, stdout);
        
		// Also output to the files
        for (int i = 0; i < argc - 1; i++) {
			if (files[i] == NULL) continue;
            fwrite(buffer, 1, bytes_read, files[i]);
        }
    }

    for (int i = 0; i < argc - 1; i++) {
		if (files[i] == NULL) continue;
        fclose(files[i]);
    }

    return 0;
}

