#include <sys/types.h>	// pipe
#include <stdlib.h>     // atoi
#include <stdio.h>      // printf
#include <string.h>     // strncpy
#include "cspipe.h"

/**
 * @file tl_remote.c
 *
 * @brief usage tl_remote \<id\> \<command\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 6.1.2015
 *
 * Program tl_remote execute command on tested router.
 * Example command: tl_remote 4 "io get bin0". Ansewer: 1.
 *
 * @param <id> Router id of your tested router.
 * @param <command> The command that will be done in the router.
 *
 * @returns 0 - Command is execute.<br>
 *          1 - Commnad is not execute.
 *
 * @cond
 */

void help(void){
	fprintf(stderr, "usage tl_remote <id> <command>\n");
}

int main(int argc, char *argv[]){
	int   fifo_fd;                    // Roura pro prijem dat
	int   router;                     // Identifikator routeru
	int   result;                     // Navratovy kod funkci
	message_remote remote_request;    // Format posilanych dat
	message_remote remote_response;   // Format posilanych dat

	// Kontrola parametru command
	if(argc != 3){
		help();
		return 1;
	}

	// Nazev serverove roury
	router = atoi(argv[1]);
	if(router <= 0){
		help();
		return 1;
	}

	// Vytvoreni zpravy
	remote_request.request = remote_process;
	strncpy(remote_request.data, argv[2], sizeof(remote_request.data));

	// Navazani spojeni
	fifo_fd = client_starting(router);
	if(fifo_fd){
		// Odelani zadosti
		result = send_mess_to_server(fifo_fd, remote_request);

		if(result){
			// Prijem odpovedi
			read_response_from_server(&remote_response);

			// Tisk odpovedi na std
			printf("%s", remote_response.data);
		}
	}

	// Ukonceni komunikace
	client_ending(router, fifo_fd);

	return remote_response.request != remote_response_ok;
}
