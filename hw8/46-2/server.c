#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "fifo_seqnum.h"

int msgid;
static void cleanup() {
	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(EXIT_FAILURE);
	}
}

int main() {
    umask(0);
	if (creat(SERVER_MSG, 0666) == -1)
	{
		perror("creat");
		exit(EXIT_FAILURE);
	}
	key_t key = ftok(SERVER_MSG, 42);
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	msgid = msgget(key, 0666 | IPC_CREAT);
	if (msgid == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	if (atexit(cleanup) == -1) {
		perror("atexit");
		exit(EXIT_FAILURE);
	}

    char clientMSG[CLI_MSG_NAME_LEN];
    int seqNum = 0;
	struct request req;
	struct response resp;
    for (;;) {
        if (msgrcv(msgid, &req, sizeof(req.pid) + sizeof(req.seqLen), 0, 0) == -1) {
            perror("msgrcv");
			exit(EXIT_FAILURE);
            continue;
        }

        snprintf(clientMSG, CLI_MSG_NAME_LEN, CLI_MSG_TEMPLATE, (long) req.pid);
        fprintf(stderr, "get request from req.pid = %d\n", req.pid);

        // Since the System V message queue never block, there is no need to deal with the situation.
		key_t cli_key = ftok(clientMSG, 42);
        if (cli_key == -1) {
            perror("ftok");
            fprintf(stderr, "Skip the request\n");
            continue;
        }
		int cli_msgid = msgget(cli_key, 0666 | IPC_CREAT);
		if (cli_msgid == -1) {
			perror("msgget");
			exit(EXIT_FAILURE);
		}
		
		resp.mtype = 1;
        resp.seqNum = seqNum;
        if (msgsnd(cli_msgid, &resp, sizeof(resp.seqNum), 0) != 0) {
            perror("msgsnd");
            continue;
        }
        seqNum += req.seqLen;
    }
}
