#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "mysig.h"

volatile sig_atomic_t flag = 1;

static void handler(int sig) {
    fprintf(stderr, "caught sig: %d\n", sig);
    flag = 0;
}

int main() {
    fprintf(stderr, "Test mysigset (press Crtl-C)\n");
    sighandler_t old_handler = mysigset(SIGINT, handler);
    while (flag);
    fprintf(stderr, "Test mysigset: Pass\n\n");
    mysigset(SIGINT, old_handler);

    fprintf(stderr, "Test mysighold (press Ctrl-C (blocked), then Ctrl-\\\\)\n");
    mysighold(SIGINT);
    flag = 1;
    old_handler = mysigset(SIGQUIT, handler);
    while (flag);
    fprintf(stderr, "Test mysighold: Pass\n\n");
    mysigset(SIGQUIT, old_handler);

    fprintf(stderr, "Test mysigrelse (SIGINT now is unblocked)\n");
    old_handler = mysigset(SIGINT, handler);
    mysigrelse(SIGINT);
    mysigset(SIGINT, old_handler);
    fprintf(stderr, "Test mysigrelse: Pass\n\n");

    fprintf(stderr, "Test mysigignore (press Ctrl-C (ignored), then Ctrl-\\\\)\n");
    mysigignore(SIGINT);
    old_handler = mysigset(SIGQUIT, handler);
    flag = 1;
    while (flag);
    mysigset(SIGQUIT, old_handler);
    fprintf(stderr, "Test mysigignore: Pass\n\n");

    fprintf(stderr, "Test mysigpause (press Ctrl-C (blocked), then Ctrl-\\\\)\n");
    old_handler = mysigset(SIGQUIT, handler);
    mysigpause(SIGQUIT);
    mysigset(SIGQUIT, old_handler);
    fprintf(stderr, "Test mysigpause: Pass\n\n");
    
    return 0;
}
