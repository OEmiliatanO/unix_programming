// sig_speed_sigwaitinfo.c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    (void) argc;

    int num_signals = atoi(argv[1]);
    pid_t child_pid;

    sigset_t mask;
    siginfo_t info;

    // Block the signals in the mask
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    if ((child_pid = fork()) == 0) {
        for (int i = 0; i < num_signals; i++) {
            kill(getppid(), SIGUSR1);             // Send signal to parent
            sigwaitinfo(&mask, &info);            // Wait for response from parent
        }
        exit(EXIT_SUCCESS);
    } else {
        for (int i = 0; i < num_signals; i++) {
            sigwaitinfo(&mask, &info);            // Wait for child signal
            kill(child_pid, SIGUSR2);             // Respond back to child
        }
        exit(EXIT_SUCCESS);
    }

    return 0;
}

