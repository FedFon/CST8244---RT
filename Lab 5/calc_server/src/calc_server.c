#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/neutrino.h>
#include "calc_message.h"

int main(void) {
	/*variables*/
	client_send_t client_message;
	server_response_t response;

	int rcvid;
	int chid;

	/*Creates a communication channel*/
	chid = ChannelCreate(0);
	if (chid == -1) {/*returns -1 on error*/
		perror("Channel could not be created");
		exit(1);
	}
	/*print the PID for the user*/
	printf("CalcServer PID: %d\n", getpid());

	while (1) {/*Infinite loop*/
		/*Receive a message from a client on the communication channel*/
		rcvid = MsgReceive(chid, (void*) &client_message,
				sizeof(client_message), NULL);
		/*set the status code to SRVR_OK to be changed depending on the situation*/
		response.statusCode = SRVR_OK;
		/*Checking for overflow. If yes sets status code to SRVR_OVERFLOW*/
		if (client_message.left_hand > (INT_MAX - client_message.right_hand)) {
			response.statusCode = SRVR_OVERFLOW;
		} else {
			/*Switch statement to perform operations based on command line arguments sent by the client (+,-,*,/)*/
			switch (client_message.operator) {
			case '+':
				response.answer = client_message.left_hand
						+ client_message.right_hand;
				break;
			case '-':
				response.answer = client_message.left_hand
						- client_message.right_hand;
				break;
			case 'x':
			case 'X':
				response.answer = client_message.left_hand
						* client_message.right_hand;
				break;
			case '/':
				if (client_message.right_hand != 0) {/*check if client is dividing by 0. If yes, set status code to SRVR_UNDEFINED*/
					response.answer = (double) client_message.left_hand
							/ (double) client_message.right_hand;
				} else {
					response.statusCode = SRVR_UNDEFINED;
				}
				break;
			default:
				/*checks if illegal operator are used. If yes, sets status code to SRVR_INVALID_OPERATOR*/
				printf("Illegal operator used");
				response.statusCode = SRVR_INVALID_OPERATOR;
				break;

			}
		}
		/*Sends the reply to the client*/
		MsgReply(rcvid, EOK, (void*) &response, sizeof(response));
	}

	/*Destroys the communication channel*/
	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
