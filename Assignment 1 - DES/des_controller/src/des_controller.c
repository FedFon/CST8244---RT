/*
 *NAME: 		des_controller.c
 *PURPOSE: 	des_controller is a server for des_inputs and a client for des_display. It will receive a Person object from des_inputs and depending on the values of that object,
 *			will send an object to des_display.
 *CREATED ON: 	2021-03-10
 *AUTHORS:		Johnathan Slaunwhite and Federico Fonseca
 */
#include "D:\QNX\Workspaces\cst8244_assign1\des_controller\src\des.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

/*void pointer function declarations*/
void *state_start();
void *state_ls();
void *state_rs();
void *state_ws();
void *state_lo();
void *state_ro();
void *state_lc();
void *state_rc();
void *state_gru();
void *state_grl();
void *state_gll();
void *state_glu();
void *state_exit();

pid_t displayPid;/*display pid*/
Display display;/*response structure*/
Person person;/*person structure*/
int coid;/*for sending messages to des_display*/
int chid;/*for creating a communication channel*/
int rcvid; /*for receiving from des_inputs*/

/*our pointer to void function will point to the state start*/
ControllerState st = state_start;

int main(int argc, char **argv) {

	/*Phase I*/
	/*Get display's PID from command-line arguments*/
	if (argc != 2) {/*If error*/
		perror("Invalid number of arguments for the controller");
		exit(EXIT_FAILURE);
	}
	displayPid = atoi(argv[1]);

	/*print controller's PID*/
	printf("des_controller PID:%d \n", getpid());

	/*Create communication channel for des_inputs to attach*/
	chid = ChannelCreate(0);
	if (chid == -1) {/*If returns error*/
		perror("Channel could not be created in the controller");
		exit(EXIT_FAILURE);
	}

	/*Attach to established communication channel des_display*/
	coid = ConnectAttach(ND_LOCAL_NODE, displayPid, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {/*If returns error*/
		perror("Channel could not connect in the controller");
		exit(EXIT_FAILURE);
	}

	/*Phase II*/
	while (1) {/*Infinite loop*/
		/*Call MsgReceive to receive Person object from des_inputs*/
		rcvid = MsgReceive(chid, &person, sizeof(person), NULL);
		if (rcvid == -1) {/*If returns error*/
			perror("Error receiving the message in the controller");
			exit(EXIT_FAILURE);
		}

		/*check received person object's state_holder*/
		if (person.state_holder == STATE_EXIT) {/*Always check for exit because it's always a valid input*/
			st = (*state_exit)();/*pointer function to point to exit state*/
			break;
		} else {/*if not exit*/
			st = (ControllerState) (*st)();/*go to wherever our pointer function is pointing to*/
			/*Reply to des_inputs*/
			MsgReply(rcvid, EOK, &person, sizeof(person));
		}
	}

	ConnectDetach(coid);/*Detach from communication channel for des_display*/
	ChannelDestroy(chid);/*Destroy communication channel for des_inputs*/

	return EXIT_SUCCESS;
}

void *state_start() {
	if (person.state_holder == INPUT_LS) {/*if LS*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_ls;/*point to state_ls*/
	} else if (person.state_holder == INPUT_RS) {/*if RS*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_rs;/*point to state_rs*/
	}
	return state_start;/*if no advancement in states return itself*/
}

void *state_ls() {
	if (person.state_holder == INPUT_GLU) {/*if GLU*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_glu;/*point to state_glu*/
	}
	return state_ls;/*if no advancement in states return itself*/
}

void *state_rs() {
	if (person.state_holder == INPUT_GRU) {/*if GRU*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_gru;/*point to state_gru*/
	}
	return state_rs;/*if no advancement in states return itself*/
}

void *state_ws() {
	if (person.state_holder == INPUT_LC && person.direction == 0) {/*if LC and its a left scan*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_lc;/*point to state_lc*/
	} else if (person.state_holder == INPUT_RC && person.direction == 1) {/*if RC and its a right scan*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_rc;/*point to state_rc*/
	}
	return state_ws;/*if no advancement in states return itself*/
}

void *state_lo() {
	if (person.state_holder == INPUT_WS && person.direction == 0) {/*if WS and its a left scan*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_ws;/*point to state_ws*/
	} else if (person.state_holder == INPUT_LC && person.direction == 1) {/*if LC and its a right scan*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_lc;/*point to state_lc*/
	}
	return state_lo;/*if no advancement in states return itself*/
}

void *state_ro() {
	if (person.state_holder == INPUT_RC && person.direction == 0) {/*if RC and its a left scan*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_rc;/*point to state_rc*/
	} else if (person.state_holder == INPUT_WS && person.direction == 1) {/*if WS and its a right scan*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_ws;/*point to state_ws*/
	}
	return state_ro;/*if no advancement in states return itself*/
}

void *state_lc() {
	if (person.state_holder == INPUT_GLL) {/*if GLL*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_gll;/*point to state_gll*/
	}
	return state_lc;/*if no advancement in states return itself*/
}

void *state_rc() {
	if (person.state_holder == INPUT_GRL) {/*if GRL*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_grl;/*point to state_grl*/
	}
	return state_rc;/*if no advancement in states return itself*/
}

void *state_gru() {
	if (person.state_holder == INPUT_RO) {/*if RO*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_ro;/*point to state_ro*/
	}
	return state_gru;/*if no advancement in states return itself*/
}

void *state_grl() {
	if (person.state_holder == INPUT_LS) {/*if a new LS*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_ls;/*point to state_ls*/
	} else if (person.state_holder == INPUT_RS) {/*if a new RS*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_rs;/*point to state_rs*/
	} else if (person.state_holder == INPUT_GLU && person.direction == 1) {/*if a GLU*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_glu;/*point to state_glu*/
	}
	return state_grl;/*if no advancement in states return itself*/
}

void *state_gll() {
	if (person.state_holder == INPUT_GRU && person.direction == 0) {/*if a GRU*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_gru;/*point to state_gru*/
	} else if (person.state_holder == INPUT_LS) {/*if a new LS*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_ls;/*point to state_ls*/
	} else if (person.state_holder == INPUT_RS) {/*if a new RS*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_rs;/*point to state_rs*/
	}
	return state_gll;/*if no advancement in states return itself*/
}

void *state_glu() {
	if (person.state_holder == INPUT_LO) {/*if LO*/
		MsgSend(coid, &person, sizeof(person), &display, sizeof(display));/*Send Message to des_display*/
		return state_lo;/*point to state_lo*/
	}
	return state_glu;/*if no advancement in states return itself*/
}

void *state_exit() {
	return state_exit;/*return itself so in the next loop we break from infinite loop in main*/
}
