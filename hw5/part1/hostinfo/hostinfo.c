#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>

int main() {
    // get hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        fprintf(stderr, "gethostname error.\n");
        return EXIT_FAILURE;
    }

    // get sysinfo
	struct utsname sysinfo;
    if (uname(&sysinfo) == -1) {
		fprintf(stderr, "uname error.\n");
        return EXIT_FAILURE;
    }

	// get hostid
    int hostid = gethostid();

    printf("Hostname: %s\n", hostname);
    printf("Kernel Version: %s\n", sysinfo.release);
    printf("Host ID: %08x\n", hostid);  // hostid already has a newline

    return EXIT_SUCCESS;
}

