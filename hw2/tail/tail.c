#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 8192

void print_last_n_lines(int fd, int n) {
    off_t file_size;
    char buffer[BUFFER_SIZE];
    int read_size;
    int line_count = 0;
    off_t pos;
    int i;

    file_size = lseek(fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Error seeking file");
        exit(EXIT_FAILURE);
    }

    pos = file_size;
    while (pos > 0 && line_count <= n) {
        read_size = (pos >= BUFFER_SIZE) ? BUFFER_SIZE : pos;
        pos -= read_size;
        lseek(fd, pos, SEEK_SET);
        
        if (read(fd, buffer, read_size) != read_size) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }

        for (i = read_size - 1; i >= 0; i--) {
            if (buffer[i] == '\n') {
                line_count++;
                if (line_count > n) {
                    pos += i + 1;
                    break;
                }
            }
        }
    }

    lseek(fd, pos, SEEK_SET);
    while ((read_size = read(fd, buffer, BUFFER_SIZE)) > 0) {
        if (write(fileno(stdout), buffer, read_size) != read_size) {
            perror("Error writing to stdout");
            exit(EXIT_FAILURE);
        }
    }
}

void usage(char *op) {
    fprintf(stderr, "Usage: %s [-n num] file\n", op);
}

int main(int argc, char *argv[]) {
    int opt;
    int num_lines = 10; // default
    int fd;
    char *file = NULL;

    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n':
                num_lines = abs(atoi(optarg));
                break;
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        perror("Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    file = argv[optind];

    fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    print_last_n_lines(fd, num_lines);

    close(fd);
    
    return 0;
}
