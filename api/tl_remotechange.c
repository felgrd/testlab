#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "cspipe.h"

/**
 * @file tl_remotechange.c
 *
 * @brief usage tl_remotechange [-p \<port\>] [-i \<ip\>]
 * [-u \<user\>] [-s \<pass\>] [-t \<protocol\>] \<id\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 29.1.2015
 *
 * Program tl_remotechange change connection parameters and reconnect
 * conection to the router.<br>
 * Example command: tl_remotechange -i 10.40.28.33 1 Change IP address of
 * connection to the router with ID 1 to 10.40.28.33. This command has not
 * a response.
 *
 * @param -p Port of connection to the router.
 * @param -i IP addres of connection to the router.
 * @param -u Username login of connection to the router.
 * @param -s Password of connection to the router.
 * @param -t Protocol of connection to the router.
 * @param <id> Router id of your tested router.
 *
 * @return 0 - Parameters change successfull.<br>
 *         1 - Parameters does not change.
 */

void help(void){
	fprintf(stderr, "usage tl_remotechange [-p <port>] [-i <ip>]" \
	" [-u <user>] [-s <pass>] [-t <protocol>] <id>\n");
}

int main(int argc, char *argv[]){
	int             i;                         // Iteracni promena
	char            parameter;                 // Rozebirani parametru
	int             router;                    // Identifikace routeru
	int             response_result;           // Vysledek odpovedi od remote serveru
	int             result;                    // Vysledek funkce
	int             num_parameters;            // Pocet zpracovanych promenych
	char            answer[PIPE_BUFFER_SIZE];  // Buffer pro opoved z routeru
	char            *commands[20];             // Pole pozadovanych zmen
  client_request  request[20];               // Prikaz k provedeni

	// Inicializace promenych
	response_result = 0;
	num_parameters = 0;

	// Rozbor parametru na prikazove radce
	while ((parameter = getopt(argc, argv, "p:i:u:s:t:")) != -1){
		switch (parameter){
			case 'i':
				request[num_parameters] = remote_change_address;
				commands[num_parameters++] = strdup(optarg);
				break;
			case 'p':
				request[num_parameters] = remote_change_port;
				commands[num_parameters++] = strdup(optarg);
				break;
			case 'u':
				request[num_parameters] = remote_change_user;
				commands[num_parameters++] = strdup(optarg);
				break;
			case 's':
				request[num_parameters] = remote_change_pass;
				commands[num_parameters++] = strdup(optarg);
				break;
			case 't':
				request[num_parameters] = remote_change_protocol;
				commands[num_parameters++] = strdup(optarg);
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

	// Provedeni zmeny prikazu
	for(i = 0; i < num_parameters; i++){

		// Odeslani zadosti remote serveru
		result = pipe_request(router, request[i], commands[i], answer);

		// Kontrola odpovedi
		if(result < 0){
			response_result |= 1;
		}

		// Uvolneni pameti
		free(commands[i]);
	}

	// Odeslani zadosti remote serveru
	result = pipe_request(router, remote_reconnect, NULL, answer);

	// Kontrola odpovedi
	if(result < 0){
		response_result |= 1;
	}

	return response_result;
}
