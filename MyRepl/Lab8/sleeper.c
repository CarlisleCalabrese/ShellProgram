/*
 * sleeper.c
 * David Owen
 */

/*
 * Get csapp.h and csapp.c from http://csapp.cs.cmu.edu/3e/code.html
 *
 * $ gcc sleeper.c csapp.c -o sleeper
 *
 * (Ignore any compliation warnings related to code in csapp.c.)
 *
 * $ ./sleeper
 *
 * ... CTRL-C to terminate and see message indicating sleeper
 * caught SIGINT.  CTRL-Z to stop and see message indicating
 * sleeper caught SIGTSTP.  "jobs" to see list of stopped jobs
 * with numerical IDs (including ./sleeper).  "fg %1" to restart
 * job with ID 1 (i.e., sleeper) and see message indicating
 * sleeper caught SIGCONT.
 */

#include "csapp.h"

int pid;

pid_t fg_pid;

void sigint_handler(int signal) {

    // Restore default behavior for next SIGINT (which will likely
    // come from call to raise at the end of this function).
    Signal(SIGINT, SIG_DFL);

    printf("\nsleeper (%d) caught SIGINT\n", pid);

    // Send SIGINT to self.  (This time won't be caught be handler,
    // will instead cause process to terminate.)
    raise(SIGINT);
}

void sigtstp_handler(int signal) {
   
    // Restore default behavior for next SIGTSTP ...
    Signal(SIGTSTP, SIG_DFL);
    
    printf("\nsleeper (%d) caught SIGTSTP\n", pid);
   
    // Send SIGTSTP to self ...
    raise(SIGTSTP);
}

void sigcont_handler(int signal) {

    // Don't need to restore default behavior of SIGCONT, because
    // default (for already running process) is to ignore it.

    // Prepare for next SIGTSTP signal.
    Signal(SIGTSTP, sigtstp_handler);

    printf("\nsleeper (%d) caught SIGCONT\n", pid);

    // Don't need to raise SIGCONT, because process is already
    // running (and would ignore it).
}

// First command line argument is number of times to sleep.  (0
// or no argument means to continue indefinitely.)  Second argument
// -q for quiet sleeper.
int main(int argc, char **argv) {
    int i, n, q;

    n = argc > 1 ? atoi(argv[1]) : 0;
    q = argc > 2 && strcmp("-q", argv[2]) == 0;
    pid = getpid();

    // Override default behavior for SIGINT, SIGTSTP and SIGCONT.
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);
    Signal(SIGCONT, sigcont_handler);

    // Sleep 1 second, wake up, go back to sleep...
    for (i = 0; n == 0 || i < n; i++) {
        Sleep(1);
        
        if (!q) {
            printf("sleeper (%d) slept %d second(s)\n", pid, i + 1);
        }
    }

    return 0;
}