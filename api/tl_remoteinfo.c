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
 * @date 29.1.2015
 *
 * Program return connection parameter of specific router. <br>
 * Example command: tl_remoteinfo -i 4. Answer: 10.40.28.32.
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
	" or [-t] <id>\n");
}

int main(int argc, char *argv[]){
	char    parameter;                  // Rozebirani parametru
	int     result;                     // Navratova hodnota funkce
	int     router;                     // Identifikator routeru
	client_request  request;               // Prikaz k provedeni
	char    answer[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru

	// Inicializace promenych
	request = remote_process;

	// Rozbor parametru na prikazove radce
	while ((parameter = getopt(argc, argv, "piust")) != -1) {
		switch (parameter) {
			case 'i':
				request = remote_status_address;
				break;
			case 'p':
				request = remote_status_port;
				break;
			case 'u':
				request = remote_status_user;
				break;
			case 's':
				request = remote_status_pass;
				break;
			case 't':
				request = remote_status_protocol;
				break;
			case '\?':
				help();
				return 1;
			case ':':
				help();
				return 1;
		}
	}

	// Kontrola pritomnosti parametru
	if(request == remote_process){
		help();
		return 1;
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

	// Odeslani zadosti remote serveru
	result = pipe_request(router, request, NULL, answer);

	// Kontrola odpovedi
	if(result){
		printf("%s", answer);
		return 0;
	}

	return 2;
}
