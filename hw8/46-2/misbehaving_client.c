#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "fifo_seqnum.h"

char clientFifo[CLI_FIFO_NAME_LEN];

static void removeFifo() {
    unlink(clientFifo);
}

int main(int argc, char **argv) {
    snprintf(clientFifo, CLI_FIFO_NAME_LEN, CLI_FIFO_TEMPLATE, (long) getpid());
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    if (atexit(removeFifo) != 0) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }
    
    struct request req;
    req.pid = getpid();
    req.seqLen = argc > 1 ? atoi(argv[1]) : 1;

    int server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (write(server_fd, &req, sizeof(struct request)) != sizeof(struct request)) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    
    // Not to open the FIFO read end. Stall the server.
    /*
    int client_fd = open(clientFifo, O_RDONLY);
    if (client_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    struct response resp;
    if (read(client_fd, &resp, sizeof(struct response)) != sizeof(struct response)) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", resp.seqNum);
    */
    while (1);
    exit(EXIT_SUCCESS);
}
