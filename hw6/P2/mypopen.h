#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define MAX_OPEN_STREAMS 256

// Structure to store file stream and associated child process PID
struct popen_entry {
    FILE *fp;
    pid_t pid;
};

static struct popen_entry popen_table[MAX_OPEN_STREAMS];  // Array to track open streams

FILE *mypopen(const char *command, const char *mode) {
    int pipe_fd[2];  // Pipe file descriptors
    pid_t pid;
    FILE *fp;

    // Create a pipe
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
            dup2(pipe_fd[1], STDOUT_FILENO);  // Redirect stdout to pipe
            close(pipe_fd[1]);  // Close the original pipe write end
        } else if (strcmp(mode, "w") == 0) {
            // Parent writes to the pipe, child reads from stdin
            close(pipe_fd[1]);  // Close write end
            dup2(pipe_fd[0], STDIN_FILENO);  // Redirect stdin to pipe
            close(pipe_fd[0]);  // Close the original pipe read end
        } else {
            fprintf(stderr, "Invalid mode %s\n", mode);
            exit(EXIT_FAILURE);
        }

        // Execute the command using the shell
        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        // If execl fails
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

    // Store the FILE* and associated PID in the popen_table
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

    // Get the file descriptor for the FILE* stream
    int fd = fileno(fp);
    if (fd < 0 || fd >= MAX_OPEN_STREAMS || popen_table[fd].fp != fp) {
        errno = EINVAL;
        return -1;
    }

    // Get the PID of the child process
    pid_t pid = popen_table[fd].pid;

    // Close the FILE* stream
    fclose(fp);

    // Wait for the child process to terminate
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        return -1;
    }

    // Clear the entry from the popen_table
    popen_table[fd].fp = NULL;
    popen_table[fd].pid = -1;

    // Return the child's exit status
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        return -1;  // Child did not exit normally
    }
}

