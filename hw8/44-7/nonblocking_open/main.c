#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_NAME "myfifo"

int main() {
    // Create a FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    printf("FIFO created: %s\n", FIFO_NAME);

    // Open the FIFO in nonblocking mode
    int fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("open");
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }

    printf("FIFO opened in nonblocking mode for reading\n");

    // Try to read from the FIFO
    char buffer[128];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
        if (errno == EAGAIN) {
            printf("Nonblocking read: No data available (EAGAIN)\n");
        } else {
            perror("read");
        }
    } else if (bytes_read == 0) {
        printf("No writers: End of file (EOF)\n");
    } else {
        buffer[bytes_read] = '\0';
        printf("Data read: %s\n", buffer);
    }

	printf("You can see the system call, open(), do not block the process.\n");

    // Clean up
    close(fd);
    unlink(FIFO_NAME);
    printf("FIFO removed and program exiting.\n");

    return 0;
}

