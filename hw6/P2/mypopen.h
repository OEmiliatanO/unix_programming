#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define MAX_OPEN_STREAMS 256

// Structure to store file pointer and associated child process PID
struct popen_entry {
    FILE *fp;
    pid_t pid;
};

// Records of file pointer and PID
static struct popen_entry popen_table[MAX_OPEN_STREAMS];

FILE *mypopen(const char *command, const char *mode) {
    int pipe_fd[2];
    pid_t pid;
    FILE *fp;

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return NULL;
    }

    // Fork the process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return NULL;
    }

    if (pid == 0) {  // Child process
        if (strcmp(mode, "r") == 0) {
            // Parent reads from the pipe, child writes to stdout
            close(pipe_fd[0]);  // Close read end
            dup2(pipe_fd[1], fileno(stdout));  // stdout <= write end
            close(pipe_fd[1]);  // Close write end
        } else if (strcmp(mode, "w") == 0) {
            // Parent writes to the pipe, child reads from stdin
            close(pipe_fd[1]);  // Close write end
            dup2(pipe_fd[0], fileno(stdin));   // stdin  <= read end
            close(pipe_fd[0]);  // Close read end
        } else {
            fprintf(stderr, "Invalid mode %s in mypopen\n", mode);
            exit(EXIT_FAILURE);
        }

        // Execute the command using the shell
        execl("/bin/sh", "sh", "-c", command, (char *) NULL);

        perror("execl");
        exit(EXIT_FAILURE);
    }

    // Parent process
    if (strcmp(mode, "r") == 0) {
        close(pipe_fd[1]);  // Close write end
        fp = fdopen(pipe_fd[0], "r");  // Convert pipe to FILE* for reading
    } else if (strcmp(mode, "w") == 0) {
        close(pipe_fd[0]);  // Close read end
        fp = fdopen(pipe_fd[1], "w");  // Convert pipe to FILE* for writing
    } else {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return NULL;
    }

    if (fp == NULL) {
        perror("fdopen");
        return NULL;
    }

    // Record the file pointer and PID
    int fd = fileno(fp);
    popen_table[fd].fp = fp;
    popen_table[fd].pid = pid;

    return fp;
}

int mypclose(FILE *fp) {
    if (fp == NULL) {
        errno = EINVAL;
        return -1;
    }

    // Get the file descriptor for the file pointer
    int fd = fileno(fp);
    if (fd < 0 || fd >= MAX_OPEN_STREAMS || popen_table[fd].fp != fp) {
        errno = EINVAL;
        return -1;
    }

    // Get the PID
    pid_t pid = popen_table[fd].pid;

    // Close the file
    fclose(fp);

    // Wait for the child process to exit
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        return -1;
    }

    // Release the entry from the popen_table
    popen_table[fd].fp = NULL;
    popen_table[fd].pid = -1;

    // Return the child's exit status
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        return -1;  // Child did not exit normally
    }
}

