#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "cspipe.h"

#define REMOTE_CHANGE_ARGC	4
#define REMOTE_CHANGE_IP	1
#define REMOTE_CHANGE_PORT	2
#define REMOTE_CHANGE_USER	3
#define REMOTE_CHANGE_PASS	4

#define COMMANDS_SIZE		REMOTE_CHANGE_ARGC + 2

int main(int argc, char *argv[]){
	int				i;								// Iteracni promena
	char			c;								// Rozebirani parametru
	int				router;							// Identifikace routeru
	int				fifo_fd;						// Roura pro prijem dat
	int				response_result;
	int				result;							// Vysledek funkce
	char			*commands[COMMANDS_SIZE];		// Pole pozadovanych zmen
	message_remote	remote_request;					// Format posilanych dat
	message_remote	remote_response;				// Format posilanych dat
	
	// Inicializace 
	response_result = 0;
		
	// Inicializace pole
	for(i = 0; i <= REMOTE_CHANGE_ARGC; i++){
		commands[i] = NULL;
	}	
	
	// Rozbor parametru na prikazove radce
	while ((c = getopt(argc, argv, "p:i:u:s:")) != -1) {
		switch (c) {
			case 'i':
				commands[REMOTE_CHANGE_IP] = strdup(optarg);
				break;
			case 'p':
				commands[REMOTE_CHANGE_PORT] = strdup(optarg);
				break;
			case 'u':
				commands[REMOTE_CHANGE_USER] = strdup(optarg);
				break;
			case 's':
				commands[REMOTE_CHANGE_PASS] = strdup(optarg);
				break;
			case '\?':
				return 1;
			case ':':
				return 1;
		}
	}
	
	// Kontrola parametru router
	if(optind >= argc || argv[optind] == NULL){
		return 1;
	}	
	
	// Nazev serverove roury
	router = atoi(argv[optind]);
	if(router <= 0){
		return 1;
	}

	// Navazani spojeni
	fifo_fd = client_starting(router);

	if(fifo_fd){
	
		// Provedeni zmeny prikazu
		for(i = 1; i <= REMOTE_CHANGE_ARGC; i++){

			if(commands[i] != NULL){

				// Sestaveni zpravy
				remote_request.request = i;
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
