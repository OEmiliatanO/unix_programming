#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_MSG_SIZE 1024
#define CLIENT_QUEUE_NAME_PREFIX "/client_queue_"

struct requestMsg {
    char clientQueueName[256];
    char pathname[256];
};

struct responseMsg {
    int status; // 0 for success, -1 for failure
    char data[MAX_MSG_SIZE];
};

// clean up child process
static void grimReaper(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// after catching the request from client
static void serveRequest(const struct requestMsg *req) {
    mqd_t clientQueue;
    int fd;
    ssize_t numRead;
    struct responseMsg resp;

    // open the message queue of client
    clientQueue = mq_open(req->clientQueueName, O_WRONLY);
    if (clientQueue == (mqd_t)-1) {
        perror("mq_open clientQueue");
        exit(EXIT_FAILURE);
    }

    // open the file required from client
    fd = open(req->pathname, O_RDONLY);
    // cannot open
    if (fd == -1) {
        resp.status = -1;
        snprintf(resp.data, MAX_MSG_SIZE, "Couldn't open file: %s", req->pathname);
        mq_send(clientQueue, (char *)&resp, sizeof(resp), 0);
        mq_close(clientQueue);
        exit(EXIT_FAILURE);
    }

    resp.status = 0;
    // sending the file
    while ((numRead = read(fd, resp.data, MAX_MSG_SIZE)) > 0) {
        if (mq_send(clientQueue, (char *)&resp, numRead + sizeof(resp.status), 0) == -1) {
            perror("mq_send");
            break;
        }
    }

    // sending the ending response
    resp.status = -1;
    if (mq_send(clientQueue, (char *)&resp, sizeof(resp.status), 0) == -1)
        perror("mq_send");

    close(fd);
    mq_close(clientQueue);
}

mqd_t serverQueue;

// clean-up function
void cleanup() {
    mq_close(serverQueue);
    mq_unlink(SERVER_QUEUE_NAME);
}

int main() {
    struct requestMsg req;
    pid_t pid;
    struct sigaction sa;

    // open a message queue of server
    serverQueue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, NULL);
    if (serverQueue == (mqd_t)-1) {
        perror("mq_open serverQueue");
        exit(EXIT_FAILURE);
    }

    // register the clean-up function
    if (atexit(cleanup) == -1) {
        perror("atexit");
        exit(EXIT_FAILURE);
    }

    // get the attribution of the message queue
    struct mq_attr attr;
    if (mq_getattr(serverQueue, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    // set up the signal handler to catch the signal from the child process
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // start monitering the message queue of server
    for (;;) {
        ssize_t bytesRead = mq_receive(serverQueue, (char *)&req, sizeof(req) + attr.mq_msgsize, NULL);
        if (bytesRead == -1) {
            if (errno == EINTR)
                continue;
            perror("mq_receive");
            break;
        }

        fprintf(stderr, "Received\n");

        // fork out a child process to process the request from the client
        pid = fork();
        if (pid == -1) {
            perror("fork");
            break;
        }

        if (pid == 0) {
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }
    }

    return 0;
}

