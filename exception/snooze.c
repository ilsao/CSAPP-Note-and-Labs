#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int signum)
{
    return;
}

unsigned int snooze(unsigned int secs)
{
	unsigned int rc = sleep(secs);

	printf("\nSlept for %d of %d secs\n", secs - rc, secs);
	return rc;
}

int main(int argc, int *argv[])
{
    if (signal(SIGINT, sighandler) == SIG_ERR) {
        fprintf(stderr, "signal error: %s\n", strerror(errno));
        exit(0);
    }

    snooze(atoi(argv[1]));
	return 0;
}