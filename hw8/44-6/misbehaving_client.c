#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "fifo_seqnum.h"

char clientFifo[CLI_FIFO_NAME_LEN];

// Function to remove the client's FIFO when the program exits
static void removeFifo() {
    unlink(clientFifo);
}

int main(int argc, char **argv) {
    snprintf(clientFifo, CLI_FIFO_NAME_LEN, CLI_FIFO_TEMPLATE, (long) getpid());
	// Create the client's FIFO
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
	
	// Register the removeFifo function to be called on program exit
    if (atexit(removeFifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }
    
	// Prepare the request structure
    struct request req;
    req.pid = getpid();
    req.seqLen = argc > 1 ? atoi(argv[1]) : 1;

	// Open the server's FIFO for writing
    int server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

	// Write the request to the server's FIFO
    if (write(server_fd, &req, sizeof(struct request)) != sizeof(struct request)) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    
    // Not to open the FIFO read end. Stall the server.

    exit(EXIT_SUCCESS);
}
