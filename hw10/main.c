#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHM_FILE "/shm_file"

struct data_t {
    int seq;
    char message[80];
};

void ErrExit(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

volatile sig_atomic_t received;
int sigseq;
void notified(int sig, siginfo_t *info, void *ucontext) {
    (void)sig; (void)ucontext;
    received = 1;
    sigseq = info->si_value.sival_int;
}

void send_data(void *shm_mem, int seq, const char *message) {
    static struct data_t data;
    data.seq = seq;
    strcpy(data.message, message);
    memcpy(shm_mem, &data, sizeof(struct data_t));
}
int notify(pid_t pid, int seqnum) {
    static union sigval seq;
    seq.sival_int = seqnum;
    return sigqueue(pid, SIGUSR1, seq);
}

void producer(void *shm_mem, const char *message, pid_t *children_pid, int data_num, int interval_ms, int consumer_num) {
    for (int i = 0; i < data_num; ++i) {
        send_data(shm_mem, i+1, message);
        for (int j = 0; j < consumer_num; ++j) {
            if (notify(children_pid[j], i+1) == -1)
                ErrExit("sigqueue");
        }
        usleep(interval_ms*1000);
    }
    for (int j = 0; j < consumer_num; ++j) {
        send_data(shm_mem, -1, ""); // sending terminal data
        if (notify(children_pid[j], -1) == -1)
            ErrExit("sigqueue");
    }
}

void consumer(int n, void *shm_mem, int buffer_size) {
    struct data_t buffer[buffer_size];
    int i = 0;
    int recv_num = 0;
    while (1) {
        if (!received) continue;
        if (sigseq == -1) break;
        received = 0;
        sigseq = 0;
        ++recv_num;
        memcpy(buffer + i, shm_mem, sizeof(struct data_t));
        i = (i + 1) % buffer_size;
    }
    //fprintf(stderr, "%d[%d] ", n, recv_num);
    ((int *)((struct data_t *)shm_mem+1))[n] = recv_num;
}

int main(int argc, char **argv) {
    if (argc != 5)
        ErrExit("./main data_num interval(ms) consumer_num buffer_size\n");

    int data_num = atoi(argv[1]);
    int interval_ms = atoi(argv[2]);
    int consumer_num = atoi(argv[3]);
    int buffer_size = atoi(argv[4]);

    if (data_num < 0 || interval_ms < 0 || consumer_num < 0 || buffer_size < 0)
        ErrExit("./main data_num interval(ms) consumer_num buffer_size\n");

    struct sigaction newact, oldact;
    if (sigemptyset(&newact.sa_mask) == -1)
        ErrExit("sigemptyset");

    newact.sa_flags = SA_SIGINFO;
    newact.sa_sigaction = notified;
    if (sigaction(SIGUSR1, &newact, &oldact) == -1)
        ErrExit("sigaction");

    int shm_fd = shm_open(SHM_FILE, O_CREAT | O_TRUNC | O_RDWR, 0666);
    const int MMAP_SIZE = sizeof(struct data_t) + sizeof(int) * consumer_num;
    if (shm_fd == -1)
        ErrExit("shm_open");
    if (ftruncate(shm_fd, MMAP_SIZE) == -1)
        ErrExit("ftruncate");

    //fprintf(stderr, "Consumer receive stat (pid[recv_num]):\n");

    pid_t children_pid[consumer_num];
    for (int i = 0; i < consumer_num; ++i) {
        children_pid[i] = fork();
        if (children_pid[i] == -1) {
            ErrExit("fork");
        } else if (children_pid[i] == 0) {
            void *shm_mem = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            if (shm_mem == MAP_FAILED) 
                ErrExit("mmap");
            consumer(i, shm_mem, buffer_size);
            if (munmap(shm_mem, MMAP_SIZE) == -1)
                ErrExit("munmap");
            return 0;
        }
    }

    void *shm_mem = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_mem == MAP_FAILED)
        ErrExit("mmap");
    const char message[] = "Hello";
    producer(shm_mem, message, children_pid, data_num, interval_ms, consumer_num);

    int tot_recv_num = 0;
    for (int i = 0; i < consumer_num; ++i) {
        int wstatus;
        while ((wstatus = waitpid(children_pid[i], NULL, WNOHANG)) != children_pid[i]) {
            if (wstatus == -1)
                ErrExit("waitpid");
            usleep(2*interval_ms*1000);
            if (notify(children_pid[i], -1) == -1)
                ErrExit("sigqueue");
        }
        tot_recv_num += ((int *)((struct data_t *)shm_mem+1))[i];
    }
    fprintf(stderr, 
"======================================\n\
Total message: %d\n\
Sum of received messages by all consumers: %d\n\
Loss rate: 1-(%d/%d)=%lf\n\
=======================================\n", 
            data_num*consumer_num, 
            tot_recv_num, 
            tot_recv_num, data_num*consumer_num,
            1.-(tot_recv_num*1./(data_num*consumer_num))
            );
    if (munmap(shm_mem, MMAP_SIZE) == -1)
        ErrExit("munmap");

    return 0;
}
