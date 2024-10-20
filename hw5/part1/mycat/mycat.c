#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: mycat filename\n");
		return EXIT_FAILURE;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Cannot open the file.\n");
		return EXIT_FAILURE;
	}
	char buf[BUF_SIZE];
	int read_size = -1;
	while ((read_size = read(fd, buf, BUF_SIZE)) > 0) {
		write(fileno(stdout), buf, read_size);
	}

	if (read_size == -1) {
		fprintf(stderr, "Read error.");
	}

	close(fd);

    return 0;
}
