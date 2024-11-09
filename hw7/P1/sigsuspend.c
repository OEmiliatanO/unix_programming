// sig_speed_sigsuspend.c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

static void handler(int sig) {
    // Handler function for signals
    (void) sig;
}

int main(int argc, char *argv[]) {
    (void) argc;
    struct sigaction sa;
    int num_signals = atoi(argv[1]);
    pid_t child_pid;

    // Set up signal handler
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    sigset_t mask, old_mask, suspend_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);
    suspend_mask = old_mask;
    sigdelset(&suspend_mask, SIGUSR1);
    sigdelset(&suspend_mask, SIGUSR2);

    // Block signals and use sigsuspend() to wait
    if ((child_pid = fork()) == 0) {
        for (int i = 0; i < num_signals; i++) {
            kill(getppid(), SIGUSR1);  // Send signal to parent
            sigsuspend(&suspend_mask);    // Wait for response from parent
        }
        exit(EXIT_SUCCESS);
    } else {
        for (int i = 0; i < num_signals; i++) {
            sigsuspend(&suspend_mask);    // Wait for child signal
            kill(child_pid, SIGUSR2);  // Respond back to child
        }
        exit(EXIT_SUCCESS);
    }

    return 0;
}
