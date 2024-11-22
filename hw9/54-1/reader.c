#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024

struct shmseg {                 /* Defines structure of shared memory segment */
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];         /* Data being transferred */
};

#define SHM_NAME "/posix_shm"
#define SEM_WRITE "/sem_write"
#define SEM_READ "/sem_read"

void errExit(const char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}

int main() {
    int shmfd, xfrs, bytes;
    struct shmseg *shmp;
    sem_t *sem_write, *sem_read;

    /* Open existing shared memory object */
    shmfd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shmfd == -1)
        errExit("shm_open");

    /* Map shared memory into address space */
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, shmfd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    /* Open semaphores */
    sem_write = sem_open(SEM_WRITE, 0);
    if (sem_write == SEM_FAILED)
        errExit("sem_open (write)");

    sem_read = sem_open(SEM_READ, 0);
    if (sem_read == SEM_FAILED)
        errExit("sem_open (read)");

    /* Transfer data from shared memory to stdout */
    for (xfrs = 0, bytes = 0; ; xfrs++) {
        if (sem_wait(sem_read) == -1)
            errExit("sem_wait");

        if (shmp->cnt == 0) /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            fprintf(stderr, "partial/failed write\n");

        if (sem_post(sem_write) == -1)
            errExit("sem_post");
    }

    /* Clean up */
    if (sem_post(sem_write) == -1)
        errExit("sem_post");

    if (sem_close(sem_write) == -1 || sem_close(sem_read) == -1)
        errExit("sem_close");

    if (munmap(shmp, sizeof(struct shmseg)) == -1)
        errExit("munmap");

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}

