#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_NAME "myfifo"

static void removeFifo() {
    unlink(FIFO_NAME);
}

int writer() {
    // Open the FIFO for writing in nonblocking mode
	int fd;
	while (1) {
		fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
		if (fd == -1) {
			perror("writer open");
			sleep(1);
		} else break;
	}

    printf("FIFO opened in nonblocking mode for writing\n");

    // Try to write to the FIFO
    const char *message = "-Hello from writer!";
    ssize_t bytes_written = write(fd, message, strlen(message));
    if (bytes_written == -1) {
        if (errno == EAGAIN) {
            printf("Nonblocking write: FIFO is full (EAGAIN)\n");
        } else {
            perror("write");
        }
    } else {
        printf("Data written: %s\n", message);
    }

    // Close the FIFO
    close(fd);
    printf("Writer exiting.\n");

    return 0;
}

int reader() {
	// Open the FIFO for reading in nonblocking mode
	int fd;
    while (1) {
		fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
		if (fd == -1) {
			perror("reader open");
			exit(EXIT_FAILURE);
		}
		else break;
	}

    printf("FIFO opened in nonblocking mode for reading\n");

    char buffer[128];
	ssize_t bytes_read;
	while ((bytes_read = read(fd, buffer, sizeof(char)*1)) == 0) {
		printf("Wait for writer...\n");
		sleep(1);
	}

    // Attempt to read from the FIFO
	while (1) {
		bytes_read = read(fd, buffer+1, sizeof(buffer) - 2);
		if (bytes_read == -1) {
			if (errno == EAGAIN) {
				printf("Nonblocking read: No data available (EAGAIN)\n");
				sleep(2);
				continue;
			} else {
				perror("read");
				break;
			}
		} else if (bytes_read == 0) {
			printf("No writers: End of file (EOF)\n");
			break;
		} else {
			buffer[bytes_read+1] = '\0';
			printf("Data read: %s\n", buffer);
		}
	}

    // Close the FIFO
    close(fd);
    printf("Reader exiting.\n");

    return 0;
}

int main() {
	if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}

	if (atexit(removeFifo) == -1) {
		perror("atexit");
		exit(EXIT_FAILURE);
	}
	
	switch (fork()) {
		case 0:
			reader();
			break;
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
		default:
			writer();
	}

	return 0;
}

