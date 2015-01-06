#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cspipe.h"

int main(int argc, char *argv[]){
	char	c;							// Rozebirani parametru
	int		fifo_fd;
	int		result;
	int		router;
	message_remote	remote_request;		// Format posilanych dat
	message_remote	remote_response;	// Format posilanych dat
		
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
		
	return 1;
}
