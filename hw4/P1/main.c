#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

void get_processes_by_user(const char *username) {
    struct passwd *pw = getpwnam(username);
    if (!pw) {
        perror("getpwnam");
        fprintf(stderr, "User %s not found.\n", username);
        return;
    }
    
    uid_t user_id = pw->pw_uid;
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {
        // Check if entry is a number (i.e., a PID)
        if (entry->d_type == DT_DIR) {
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr != '\0') {
                continue; // Not a number
            }

            // Construct the path to the status file
            char status_file_path[1024];
            snprintf(status_file_path, sizeof(status_file_path), "/proc/%s/status", entry->d_name);
            FILE *status_file = fopen(status_file_path, "r");
            if (status_file) {
                char line[256];
                uid_t uid = 0;
                char comm[1024] = "";

                // Read the status file
                while (fgets(line, sizeof(line), status_file)) {
                    if (sscanf(line, "Uid:\t%d", &uid) == 1) {
                        // Found the UID
                    }
                    if (sscanf(line, "Name:\t%s", comm) == 1) {
                        // Found the command name
                    }
                }
                fclose(status_file);

                // Check if the UID matches the user's UID
                if (uid == user_id) {
                    printf("PID: %ld, Command: %s\n", pid, comm);
                }
            } else {
                // Handle potential race condition
                if (errno != ENOENT) {
                    perror("fopen");
                }
            }
        }
    }
    closedir(proc_dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return EXIT_FAILURE;
    }

    get_processes_by_user(argv[1]);
    return EXIT_SUCCESS;
}

