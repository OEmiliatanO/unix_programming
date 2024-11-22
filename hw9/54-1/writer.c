#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    /* Create shared memory object and set size */
    shmfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (shmfd == -1)
        errExit("shm_open");

    if (ftruncate(shmfd, sizeof(struct shmseg)) == -1)
        errExit("ftruncate");

    /* Map shared memory into address space */
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    /* Create and initialize semaphores */
    sem_write = sem_open(SEM_WRITE, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);
    if (sem_write == SEM_FAILED)
        errExit("sem_open (write)");

    sem_read = sem_open(SEM_READ, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
    if (sem_read == SEM_FAILED)
        errExit("sem_open (read)");

    printf("Ctrl-D to exit.\n");

    /* Transfer data from stdin to shared memory */
    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
        if (sem_wait(sem_write) == -1)
            errExit("sem_wait");

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1)
            errExit("read");

        if (sem_post(sem_read) == -1)
            errExit("sem_post");

        if (shmp->cnt == 0) /* End of file */
            break;
    }

    /* Clean up */
    if (sem_wait(sem_write) == -1)
        errExit("sem_wait");

    if (sem_close(sem_write) == -1 || sem_close(sem_read) == -1)
        errExit("sem_close");

    if (sem_unlink(SEM_WRITE) == -1 || sem_unlink(SEM_READ) == -1)
        errExit("sem_unlink");

    if (munmap(shmp, sizeof(struct shmseg)) == -1)
        errExit("munmap");

    if (shm_unlink(SHM_NAME) == -1)
        errExit("shm_unlink");

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}

