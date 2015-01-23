#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cspipe.h"

/**
 * @file tl_remoteinfo.c
 *
 * @brief usage tl_remoteinfo [-p] or [-i] or [-u] or [-s] or [-t] \<id\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 6.1.2015
 *
 * Program answer parameter of connection specific router.<br>
 * Example command: tl_remoteinfo -p 4. Answer: 22.
 *
 * @param -p Port of connection to he router.
 * @param -i IP addres of connection to the router.
 * @param -u Username login of connection to the router.
 * @param -s Password of connection to the router.
 * @param -t Protocol of connection to the router.
 * @param <id> Router id of your tested router.
 *
 * @return 0 -> Answer is valid.<br>
 *         1 -> Parameter is not valid.<br>
 *         2 -> Bad answer from remote program.
 *
 * @cond
 */

void help(void){
	fprintf(stderr, "usage tl_remoteinfo [-p] or [-i] or [-u] or [-s]"\
	" or [-t] <id>");
}

int main(int argc, char *argv[]){
	char  c;                          // Rozebirani parametru
	int   fifo_fd;                    // Deskriptor roury
	int   result;                     // Navratova hodnota funkce
	int   router;                     // Identifikator routeru
	message_remote  remote_request;   // Format posilanych dat
	message_remote  remote_response;  // Format posilanych dat

	// Rozbor parametru na prikazove radce
	while ((c = getopt(argc, argv, "piust")) != -1) {
		switch (c) {
			case 'i':
				remote_request.request = remote_status_address;
				break;
			case 'p':
				remote_request.request = remote_status_port;
				break;
			case 'u':
				remote_request.request = remote_status_user;
				break;
			case 's':
				remote_request.request = remote_status_pass;
				break;
			case 't':
				remote_request.request = remote_status_protocol;
				break;
			case '\?':
				help();
				return 1;
			case ':':
				help();
				return 1;
		}
	}

	// Kontrola parametru router
	if(optind >= argc || argv[optind] == NULL){
		help();
		return 1;
	}

	// Nazev serverove roury
	router = atoi(argv[optind]);
	if(router <= 0){
		help();
		return 1;
	}

	// Navazani spojeni
	fifo_fd = client_starting(router);
	if(fifo_fd){
		// Odelani zadosti
		result = send_mess_to_server(fifo_fd, remote_request);

		// Prijem odpovedi
		if(result){
			read_response_from_server(&remote_response);
		}
	}

	// Ukonceni komunikace
	client_ending(router, fifo_fd);

	// Tisk vysledku
	if(remote_response.request == remote_response_ok){
		printf("%s", remote_response.data);
		return 0;
	}

	return 2;
}
