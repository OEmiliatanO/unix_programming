#include <stdio.h>
#include <signal.h>
#include "mysig.h"

static void handler(int sig) {
    fprintf(stderr, "caught sig: %d\n", sig);
}

int main() {
    sighandler_t old_handler = mysigset(SIGINT, handler);
    (void) old_handler;
    while (1) {}
    return 0;
}
