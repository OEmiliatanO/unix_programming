#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define CLIENT_QUEUE_NAME_PREFIX "/client_queue_"
#define MAX_MSG_SIZE 1024

struct requestMsg {
    char clientQueueName[256];
    char pathname[256];
};

struct responseMsg {
    int status; // 0 for success, -1 for end
    char data[MAX_MSG_SIZE];
};

char clientQueueName[256];
mqd_t serverQueue, clientQueue;

// clean-up function
void cleanup() {
    mq_close(clientQueue);
    mq_unlink(clientQueueName);
}

int main(int argc, char *argv[]) {
    struct requestMsg req;
    struct responseMsg resp;
    ssize_t bytesRead;
    int numMsgs = 0, totBytes = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // the name of the queue of client
    snprintf(clientQueueName, sizeof(clientQueueName), "%s%ld", CLIENT_QUEUE_NAME_PREFIX, (long)getpid());

    // open the message queue of client
    clientQueue = mq_open(clientQueueName, O_CREAT | O_RDONLY, 0644, NULL);
    if (clientQueue == (mqd_t)-1) {
        perror("mq_open clientQueue");
        exit(EXIT_FAILURE);
    }

    // register the clean-up function
    if (atexit(cleanup) == -1) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    // get the attributions of message queue
    struct mq_attr attr;
    if (mq_getattr(clientQueue, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    // open the message queue of server
    serverQueue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (serverQueue == (mqd_t)-1) {
        perror("mq_open serverQueue");
        mq_unlink(clientQueueName);
        exit(EXIT_FAILURE);
    }
    
    // send the name of queue of client and the file client required
    strncpy(req.clientQueueName, clientQueueName, sizeof(req.clientQueueName) - 1);
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    if (mq_send(serverQueue, (char *)&req, sizeof(req), 0) == -1) {
        perror("mq_send");
        mq_close(clientQueue);
        mq_unlink(clientQueueName);
        exit(EXIT_FAILURE);
    }

    // receive the file
    for (numMsgs = 1; ; ++numMsgs) {
        bytesRead = mq_receive(clientQueue, (char *)&resp, sizeof(resp) + attr.mq_msgsize, NULL);
        if (resp.status == -1) {
            break;
        }
        totBytes += bytesRead - sizeof(resp.status);
    }

    printf("Received %d bytes (%d messages)\n", totBytes, numMsgs);

    return 0;
}

