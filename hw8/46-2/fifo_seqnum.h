#define SERVER_MSG "/tmp/seqnum_sv.msg"
#define CLI_MSG_TEMPLATE "/tmp/seqnum_cl.%ld.msg"
#define CLI_MSG_NAME_LEN (sizeof(CLI_MSG_TEMPLATE) + 20)

struct request {
	long mtype;
    pid_t pid;
    int seqLen;
};

struct response {
	long mtype;
    int seqNum;
};
