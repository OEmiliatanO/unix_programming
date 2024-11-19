#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: cp file file_copy\n");
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd2, statbuf.st_size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    void *q = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (q == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    void *p = mmap(NULL, statbuf.st_size, PROT_WRITE, MAP_SHARED, fd2, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    memcpy(p, q, sizeof(char) * statbuf.st_size);

    if (munmap(q, statbuf.st_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    if (munmap(p, statbuf.st_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    return 0;
}
