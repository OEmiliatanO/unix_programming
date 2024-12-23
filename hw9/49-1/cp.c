#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char **argv) {
    // wrong usage
    if (argc != 3) {
        fprintf(stderr, "Usage: cp file file_copy\n");
    }

    // open the source (first argument)
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

    // open the destination (second argument)
    int fd2 = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    // resize the file
    if (ftruncate(fd2, statbuf.st_size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // share the memory of the source file
    void *q = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (q == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // share the memory of the destination file
    void *p = mmap(NULL, statbuf.st_size, PROT_WRITE, MAP_SHARED, fd2, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // copy data
    memcpy(p, q, sizeof(char) * statbuf.st_size);

    // clean up
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
