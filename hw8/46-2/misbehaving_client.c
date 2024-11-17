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

int main(int argc, char **argv) {
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
    
    // Only send request to stall the server
    if (msgsnd(msgid, &req, sizeof(req.pid) + sizeof(req.seqLen), 0) != 0) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
