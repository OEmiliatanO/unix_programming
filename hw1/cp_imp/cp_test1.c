#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

void usage(const char *program_name) {
    fprintf(stderr, "Usage: %s SOURCE DEST\n", program_name);
    exit(EXIT_FAILURE);
}

const char src_filemode[] = "rb";
const char dest_filemode[] = "wb";

int main(int argc, char *argv[]) {
    FILE *src_fp, *dest_fp;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    if (argc < 3) {
        usage(argv[0]);
    }
	
	// open SOURCE
	src_fp = fopen(argv[1], src_filemode);
	if (src_fp == NULL) {
		sprintf(buffer, "Cannot open file %s", argv[1]);
		perror(buffer);
		exit(EXIT_FAILURE);
	}
	// open DEST
	dest_fp = fopen(argv[2], dest_filemode);
	if (dest_fp == NULL) {
		sprintf(buffer, "Cannot open file %s", argv[2]);
		perror(buffer);
		exit(EXIT_FAILURE);
	}

	while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_fp)) > 0) {
        fwrite(buffer, 1, bytes_read, dest_fp);
    }

	fclose(src_fp);
	fclose(dest_fp);

    return 0;
}

