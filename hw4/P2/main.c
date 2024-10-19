#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

typedef struct Process {
    pid_t pid;
    pid_t ppid;
    char cmd[256];
    struct Process *children;
    struct Process *next;
} Process;

Process *process_list = NULL;

Process* create_process(pid_t pid, pid_t ppid, const char *cmd) {
    Process *proc = malloc(sizeof(Process));
    if (!proc) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    proc->pid = pid;
    proc->ppid = ppid;
    strncpy(proc->cmd, cmd, sizeof(proc->cmd) - 1);
    proc->cmd[sizeof(proc->cmd) - 1] = '\0';
    proc->children = NULL;
    proc->next = NULL;
    return proc;
}

void add_to_list(Process *proc) {
    proc->next = process_list;
    process_list = proc;
}

void add_child(Process *parent, Process *child) {
    if (!parent->children) {
        parent->children = child;
    } else {
        Process *current = parent->children;
        while (current->next) {
            current = current->next;
        }
        current->next = child;
    }
}

void read_process_status(pid_t pid) {
    char status_file_path[256];
    snprintf(status_file_path, sizeof(status_file_path), "/proc/%d/status", pid);
    FILE *status_file = fopen(status_file_path, "r");
    if (!status_file) {
        if (errno != ENOENT) {
            perror("fopen");
        }
        return;
    }

    pid_t ppid = -1;
    char cmd[256] = "";

    char line[256];
    while (fgets(line, sizeof(line), status_file)) {
        if (sscanf(line, "PPid:\t%d", &ppid) == 1) {
            // Found the parent PID
        }
        if (sscanf(line, "Name:\t%s", cmd) == 1) {
            // Found the command name
        }
    }
    fclose(status_file);

	Process *proc = create_process(pid, ppid, cmd);
	add_to_list(proc);
}

void build_process_tree() {
    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char *endptr;
            pid_t pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr != '\0') {
                continue; // Not a number
            }
            read_process_status(pid);
        }
    }
    closedir(proc_dir);
}

void print_process_tree(Process *proc, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("%d (%s)\n", proc->pid, proc->cmd);
    
    Process *child = proc->children;
    while (child) {
        print_process_tree(child, depth + 1);
        child = child->next;
    }
}

void free_process_tree(Process *proc) {
    if (proc) {
        free_process_tree(proc->children);
        free(proc);
    }
}

void link_children(Process* cur) {
	Process *potential_child = process_list;
	Process *last = NULL;
	Process *nex = potential_child ? potential_child->next : NULL;
	while (potential_child) {
		int added = 0;
		if (potential_child->ppid == cur->pid && potential_child->pid != cur->pid) {
			added = 1;
			if (last) last->next = nex;
			potential_child->next = NULL;
			add_child(cur, potential_child);
		}
		if (!added) last = potential_child;
		potential_child = nex;
		if (potential_child) nex = potential_child->next;
	}

	for (Process* child = cur->children; child; child=child->next) link_children(child);
}

int main() {
    build_process_tree();

    Process *cur = process_list, *last = NULL;
	// find pid = 1
	while (cur && cur->pid != 1) {
		last = cur;
		cur = cur->next;
	}
	if (cur && cur->pid == 1) {
		if (last) last->next = cur->next;
		cur->next = process_list;
		process_list = cur;
	} else {
		perror("Cannot find init process.");
        exit(EXIT_FAILURE);
	}

    // Link children to their parents
    link_children(cur);

    // Print process tree starting from init (PID 1)
    Process *init_process = process_list; 
    while (init_process && init_process->pid != 1) {
        init_process = init_process->next;
    }

    if (init_process) {
        print_process_tree(init_process, 0);
    } else {
        printf("No init process found.\n");
    }

    free_process_tree(process_list);
    return EXIT_SUCCESS;
}

