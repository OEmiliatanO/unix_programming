#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "fifo_seqnum.h"

char clientMsg[CLI_MSG_NAME_LEN];

int cli_msgid;

// Cleanup function to remove the client's message queue on program exit
static void cleanup() {
	if (msgctl(cli_msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
	// Generate a unique name for the client's message queue file
    snprintf(clientMsg, CLI_MSG_NAME_LEN, CLI_MSG_TEMPLATE, (long) getpid());
	
	// Generate a key for the server's message queue
	key_t key = ftok(SERVER_MSG, 42);
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	// Access or create the server's message queue
	int msgid = msgget(key, 0666 | IPC_CREAT);
	if (msgid == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	// Prepare the request structure
	struct request req;
	req.mtype = 1;
    req.pid = getpid();
    req.seqLen = argc > 1 ? atoi(argv[1]) : 1;

	// Create a file for the client's message queue
    if (creat(clientMsg, 0666) == -1) {
		perror("creat");
		exit(EXIT_FAILURE);
	}

	// Send the request to the server's message queue
    if (msgsnd(msgid, &req, sizeof(req.pid) + sizeof(req.seqLen), 0) != 0) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

	// Generate a key for the client's message queue
	key_t cli_key = ftok(clientMsg, 42);
	if (cli_key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	// Access or create the client's message queue
	cli_msgid = msgget(cli_key, 0666 | IPC_CREAT);
	if (cli_msgid == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	// Register the cleanup function to run on program exit
	if (atexit(cleanup) == -1) {
		perror("atexit");
		exit(EXIT_FAILURE);
	}

	// Receive the response from the client's message queue
    struct response resp;
    ssize_t recv_size = msgrcv(cli_msgid, &resp, sizeof(resp.seqNum), 0, 0);
    if (recv_size != sizeof(resp.seqNum)) {
        fprintf(stderr, "recv size = %ld, sizeof response = %ld, errno = %d\n", recv_size, sizeof(struct response), errno);
        fprintf(stderr, "%ld, %d\n", resp.mtype, resp.seqNum);
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }


    printf("%d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}
