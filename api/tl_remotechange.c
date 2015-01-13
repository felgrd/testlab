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
 * @date 6.1.2015
 * 
 * Program tl_remotechange change connection parameters and again
 * connects to the router.
 * 
 * @param -p Port of connection to he router.
 * @param -i IP addres of connection to the router.
 * @param -u Username login of connection to the router.
 * @param -s Password of connection to the router.
 * @param -t Protocol of connection to the router.
 * @param <id> Router id of your tested router.
 *  
 * @return 0 - Parameters change successfull.<br>
 *         1 - Parameters do not change.
 */
 
void help(void){
	fprintf(stderr, "usage tl_remotechange [-p <port>] [-i <ip>]" \
	" [-u <user>] [-s <pass>] [-t <protocol>] <id>");
}

int main(int argc, char *argv[]){
	int				i;								// Iteracni promena
	char			c;								// Rozebirani parametru
	int				router;							// Identifikace routeru
	int				fifo_fd;						// Roura pro prijem dat
	int				response_result;
	int				result;							// Vysledek funkce
	int				num_parameters;					// Pocet zpracovanych promenych
	client_request	request[20];					// Prikaz k provedeni
	char			*commands[20];					// Pole pozadovanych zmen
	message_remote	remote_request;					// Format posilanych dat
	message_remote	remote_response;				// Format posilanych dat
	
	// Inicializace 
	response_result = 0;
	num_parameters = 0;
		
	// Rozbor parametru na prikazove radce
	while ((c = getopt(argc, argv, "p:i:u:s:t:")) != -1) {
		switch (c) {
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

	// Navazani spojeni
	fifo_fd = client_starting(router);

	if(fifo_fd){
	
		// Provedeni zmeny prikazu
		for(i = 0; i < num_parameters; i++){

			// Sestaveni zpravy
			remote_request.request = request[i];
			strncpy(remote_request.data, commands[i], \
			sizeof(remote_request.data));

			// Odeslani zpravy
			result = send_mess_to_server(fifo_fd, remote_request);
			
			// Prijem odpovedi
			if(result){
				read_response_from_server(&remote_response);					
			}			
				
			// Kontrola odpovedi
			if(remote_response.request == remote_response_error){
				response_result |= 1;
			}				
		}
	
		// Znovu pripojeni k routeru
		remote_request.request = remote_reconnect;
		
		// Odeslani zpravy
		result = send_mess_to_server(fifo_fd, remote_request);		
		
		// Prijem odpovedi
		if(result){
			read_response_from_server(&remote_response);
		}	
		
		// Kontrola odpovedi
		if(remote_response.request == remote_response_error){
			response_result |= 1;
		}
	}
	
	// Ukonceni komunikace
	client_ending(router, fifo_fd);
	
	return response_result;	
}
