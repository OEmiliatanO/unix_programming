#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

void list_processes_with_file(const char *filename) {
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    char fd_path[PATH_MAX];
    char link_target[PATH_MAX];

    // Iterate through all process in /proc
    while ((entry = readdir(proc_dir)) != NULL) {
        // Check if the entry is a valid PID (digits only)
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            // Construct the path to the file descriptor directory
            snprintf(fd_path, sizeof(fd_path), "/proc/%s/fd", entry->d_name);
            DIR *fd_dir = opendir(fd_path);
            if (!fd_dir) { // Cannot open the directory
                continue;
            }

            struct dirent *fd_entry;
            // Iterate through all file descriptors
            while ((fd_entry = readdir(fd_dir)) != NULL) {
                // Skip . and ..
                if (strcmp(fd_entry->d_name, ".") == 0 || strcmp(fd_entry->d_name, "..") == 0) {
                    continue;
                }

                // Construct the full path to the symlink
                char full_link[2*PATH_MAX];
                snprintf(full_link, sizeof(full_link), "%s/%s", fd_path, fd_entry->d_name);

                // Read the symlink target
                ssize_t len = readlink(full_link, link_target, sizeof(link_target) - 1);
                if (len != -1) {
                    link_target[len] = '\0';

                    if (strcmp(link_target, filename) == 0) {
                        printf("PID: %s has the file open: %s\n", entry->d_name, link_target);
                    }
                }
            }

            closedir(fd_dir);
        }
    }

    closedir(proc_dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file-path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    list_processes_with_file(filename);

    return 0;
}

