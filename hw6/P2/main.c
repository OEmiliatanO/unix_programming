#include <stdio.h>
#include "mypopen.h"

int main() {
    FILE *fp;
    char buffer[1024];

    // Using popen() to run a command and read its output
    fp = mypopen("ls -la", "r");
    if (fp == NULL) {
        perror("popen failed");
        return 1;
    }

    // Read the command output and print it to the console
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    // Close the stream and get the command's exit status
    int status = mypclose(fp);
	if (status < 0) {
		fprintf(stderr, "Child exit abnormally\n");
	}

    return 0;
}
