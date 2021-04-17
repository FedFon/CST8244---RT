#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include "D:\QNX\Workspaces\cst8244_lab05.ws\calc_server\src\calc_message.h"

int main(int argc, char **argv) {
	/*variables*/
	client_send_t msg_send;
	server_response_t msg_receive;

	int coid;

	/*if the number of command line arguments is not enough*/
	if (argc != 5) {
		perror("Invalid amount of arguments");
		exit(1);
	}

	pid_t srvrpid = atoi(argv[1]);

	/*grabbing command line arguments for the operations*/
	msg_send.left_hand = atoi(argv[2]);
	msg_send.operator = argv[3][0];
	msg_send.right_hand = atoi(argv[4]);

	//server pid

	/*create a connection to an already existing communication channel*/
	coid = ConnectAttach(ND_LOCAL_NODE, srvrpid, 1, _NTO_SIDE_CHANNEL, 0);
	/*if ConnectAttach returns error*/
	if (coid == -1) {
		printf("MsgSend had an error.\n");
		exit(1);
	}

	/*Send a message using the communication channel*/
	if (MsgSend(coid, &msg_send, sizeof(msg_send), &msg_receive,
			sizeof(msg_receive)) == -1) {
		/*enters if statement if error*/
		fprintf(stderr, "Error during MsgSend\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	/*Checking the status codes returned by the server*/
	if (msg_receive.statusCode != SRVR_OK) {/*if SRVR_OK is not returned*/
		/*Checks to see if status code is either SRVR_INVALID_OPERATOR, SRVR_UNDEFINED or SRVR_OVERFLOW and adjusts the output to tell the user*/
		if (msg_receive.statusCode == SRVR_INVALID_OPERATOR) {
			printf("Error message from server: INVALID OPERATOR: %c\n\n",
					msg_send.operator);
		} else if (msg_receive.statusCode == SRVR_UNDEFINED) {
			printf("Error message from server: UNDEFINED: %d %c %d\n\n",
					msg_send.left_hand, msg_send.operator, msg_send.right_hand);
		} else if (msg_receive.statusCode == SRVR_OVERFLOW) {
			printf("Error message from server: OVERFLOW: %d %c %d\n\n",
					msg_send.left_hand, msg_send.operator, msg_send.right_hand);
		}
		exit(1);
	} else {/*SRVR_OK is returned*/

		printf("The server has calculated the result of %d %c %d as %.2f\n\n",
				msg_send.left_hand, msg_send.operator, msg_send.right_hand,
				msg_receive.answer);

	}

	/*Detach the communication channel*/
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
