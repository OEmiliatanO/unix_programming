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
	FILE *fp = fopen("/etc/hostid", "r");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open /etc/hostid\n");
        return EXIT_FAILURE;
    }
    
    char hostid[32];
    if (fgets(hostid, sizeof(hostid), fp) == NULL) {
        fprintf(stderr, "Cannot get hostid from /etc/hostid\n");
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);

    printf("Hostname: %s\n", hostname);
    printf("Kernel Version: %s\n", sysinfo.release);
    printf("Host ID: %s", hostid);  // hostid already has a newline

    return EXIT_SUCCESS;
}

