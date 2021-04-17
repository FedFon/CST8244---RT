/*
 ** partB_sigproc.c -- Handles processes
 *

 *
 * Modified by: fons0017@algonquinlive.com
 *
 * Usage:
 *	Handler is implemented to handle the SIGUSR1 and makes parent processes wait for child processes depeding on how many there are
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void sigusr_handler();

volatile sig_atomic_t usr1Happened = 0;

int main(void) {

	struct sigaction sa;/*sigaction struct*/

	sa.sa_handler = sigusr_handler;/*setting handler*/
	sa.sa_flags = 0; /*or SA_RESTART*/
	sigemptyset(&sa.sa_mask);/*remove all signals*/

	sigaction(SIGUSR1, &sa, NULL);/*installing signal SIGUSR1 to handler*/

	/*variables*/
	int rv;
	int waitNumber = 0;
	int numChildren = 0;

	/*get input from user*/
	printf("Enter the number of children:\n");
	scanf("%d", &numChildren);

	waitNumber = numChildren;

	pid_t pid;

	printf("PID = %d: Parent running...\n", getpid());

	/* forking depending how many children are desired */
	while (numChildren != 0) {
		numChildren -= 1;

		pid = fork();

		if (pid < 0) {
			perror("fork"); /* something went wrong */
			exit(1); /* parent exits */
		} else if (pid == 0) {
			printf("PID = %d: Child running...\n", getpid());
			for (;;) {
				if (usr1Happened == 1) {
					printf("PID = %d: Child received USR1.\n", getpid());
					printf("PID = %d: Child exiting.\n", getpid());
					break;
				}
			}
			exit(rv);

		}
	}

	/*loop that waits for every child process*/
	while (waitNumber != 0) {
		int a = wait(&rv);
		if (a > 0) {
			--waitNumber;
		}
	}

	printf("PID = : Children finished, parent exiting.\n", getpid());
	return 0;
}

void sigusr_handler() {/*handler just sets usrHappened to 1*/
	usr1Happened = 1;
}
