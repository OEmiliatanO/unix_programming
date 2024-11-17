#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "fifo_seqnum.h"

// A flag to indicate that the timer has expired
volatile sig_atomic_t skip;

// Handle the signal from alarm
void handle_alarm(int sig) {
    (void)sig;
    skip = 1;
    fprintf(stderr, "Timer expired! Skip this client\n");
}

int main() {
    // Register the handler
    struct sigaction sa;
    sa.sa_handler = handle_alarm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

	// Ensure the FIFO can be created with proper permissions
    umask(0);
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
	
	// Open the server FIFO for reading
    int server_fd = open(SERVER_FIFO, O_RDONLY);
    if (server_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
	
	// Open a dummy write descriptor to avoid the EOF
    int dummy_fd = open(SERVER_FIFO, O_WRONLY);
    if (dummy_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

	// Ignore SIGPIPE signals to prevent termination on write errors
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    char clientFIFO[CLI_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0;
    for (;;) {
		// Read a request from the client
        if (read(server_fd, &req, sizeof(struct request)) != sizeof(struct request)) {
            perror("read");
            continue;
        }
		
		// Generate the client's FIFO name
        snprintf(clientFIFO, CLI_FIFO_NAME_LEN, CLI_FIFO_TEMPLATE, (long) req.pid);

		// Set an alarm for 5 seconds to handle potential blocking
        alarm(5);
        int clientFd = open(clientFIFO, O_WRONLY); // Open the client's FIFO for writing
		alarm(0); // Cancel the timer
		
		// Check if the timer expired
        if (skip) {
            skip = 0;
            continue;
        }
		// Check if opening the client's FIFO failed
        if (clientFd == -1) {
            perror("open");
        }
		
		// Write the response to the client's FIFO
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
            perror("write");
            continue;
        }
		
        if (close(clientFd) == -1) perror("close");
        seqNum += req.seqLen;
    }
}
