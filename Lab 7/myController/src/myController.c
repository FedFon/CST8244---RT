/*
 *NAME: 		myController.c
 *PURPOSE: 	For this lab myController.c is the server for the resource manager created by myDevice.c
 *CREATED ON: 	2021-04-05
 *AUTHORS:		Federico Fonseca
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
	struct _pulse pulse;
	char msg[255];
} my_message_t;

int main(void) {
	name_attach_t *attach_name; /*for name*/
	FILE *file;/*file name*/
	char fileStatus[25];
	my_message_t message;
	int rcvid;

	/*configure as a server*/
	attach_name = name_attach(NULL, "mydevice", 0);/*register device name*/

	if (attach_name == NULL) {/*if error*/
		perror("Error in the name_attach()\n");
		exit(EXIT_FAILURE);
	}

	file = fopen("/dev/local/mydevice", "r");/*open the device*/

	if (file == NULL) {/*if there is an error opening the file*/
		perror("Error opening the file.\n");
		exit(EXIT_FAILURE);
	}

	fscanf(file, "%s %s", fileStatus, message.msg);/*scan the status*/

	if (strcmp(fileStatus, "status") == 0) {/*if is equal to*/
		printf("STATUS: %s\n", message.msg);
		fclose(file);
	}

	if (strcmp(message.msg, "closed") == 0) {/*if is equal to*/
		name_detach(attach_name, 0);
		fclose(file);
		exit(EXIT_SUCCESS);
	}

	while (1) {/*infinite loop*/

		rcvid = MsgReceivePulse(attach_name->chid, &message.msg,
				sizeof(message.msg), NULL);/*assign msgreceive pulse to rcvid*/
		if (rcvid == -1) {/*if there is an error*/
			perror(
					"There was an error in the MsgReceivePulse() in myController.c\n");
			exit(EXIT_FAILURE);
		}

		if (rcvid == 0) {/*if received a pulse*/
			if (message.pulse.code == MY_PULSE_CODE) {
				printf("Small Integer: %d\n", message.pulse.value.sival_int);

				file = fopen("/dev/local/mydevice", "r");/*open the device*/

				if (file == NULL) {/*if there is an error opening the file*/
					perror("Error opening the file.\n");
					exit(EXIT_FAILURE);
				}

				fscanf(file, "%s %s", fileStatus, message.msg);/*scan the status*/

				if (strcmp(fileStatus, "status") == 0) {/*if is equal to*/
					printf("STATUS: %s", message.msg);
				}

				if (strcmp(message.msg, "closed") == 0) {/*if is equal to*/
					name_detach(attach_name, 0);/*detach from device*/
					fclose(file);/*close fd*/
					exit(EXIT_SUCCESS);
				}

			} else {/*if there is an error*/
				perror("Error with message.pulse.code in myController.c\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	name_detach(attach_name, 0);/*detach from device*/

	return EXIT_SUCCESS;
}
