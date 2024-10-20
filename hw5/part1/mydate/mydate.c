#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUF_SIZE 100

int main() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

	if (tm_info == NULL) {
		fprintf(stderr, "Cannot obtain the local time.\n");
		return EXIT_FAILURE;
	}

    char buffer[BUF_SIZE];
    if (strftime(buffer, sizeof(buffer), "%b %d(%a), %Y %I:%M %p", tm_info) == 0) {
		fprintf(stderr, "Buffer overflow.\n");
		return EXIT_FAILURE;
	}

    printf("%s\n", buffer);

    return 0;
}

