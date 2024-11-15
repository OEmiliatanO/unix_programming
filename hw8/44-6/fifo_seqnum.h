#define SERVER_FIFO "/tmp/seqnum_sv"
#define CLI_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"
#define CLI_FIFO_NAME_LEN (sizeof(CLI_FIFO_TEMPLATE) + 20)

struct request {
    pid_t pid;
    int seqLen;
};

struct response {
    int seqNum;
};
