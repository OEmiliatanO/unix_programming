#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mysig.h"

sighandler_t mysigset(int sig, sighandler_t handler) {
    struct sigaction new_action, old_action;

    // Set up the new handler
    new_action.sa_handler = handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    if (sigaction(sig, &new_action, &old_action) == -1) {
        perror("sigaction");
        return SIG_ERR;
    }

    // Return the old handler
    return old_action.sa_handler;
}

int mysighold(int sig) {
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, sig);

    // Block the specified signal
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        return -1;
    }

    return 0;
}

int mysigrelse(int sig) {
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, sig);

    // Unblock the specified signal
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        return -1;
    }

    return 0;
}

int mysigignore(int sig) {
    struct sigaction new_action;

    // Set the handler to SIG_IGN to ignore the signal
    new_action.sa_handler = SIG_IGN;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    if (sigaction(sig, &new_action, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    return 0;
}

int mysigpause(int sig) {
    sigset_t mask;

    // Create a mask with only the specified signal unblocked
    sigprocmask(SIG_BLOCK, NULL, &mask); // Get the current mask
    sigdelset(&mask, sig);                // Unblock the specified signal

    // Wait for a signal using sigsuspend with the modified mask
    return sigsuspend(&mask);
}
