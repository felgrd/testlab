#include <sys/types.h>		// pipe
#include <stdlib.h>			// atoi
#include <stdio.h>			// printf
#include <string.h>
#include "cspipe.h"

int main(int argc, char *argv[]){
	int		fifo_fd;						// Roura pro prijem dat
	int		router;
	int		result;
	message_remote remote_request;			// Format posilanych dat
	message_remote remote_response;			// Format posilanych dat
	
	// Kontrola parametru command
	if(argc != 2){
		fprintf(stderr, "Bad number of parameters\n");
		return 1;
	}	
	
	// Nazev serverove roury
	router = atoi(argv[1]);
	if(router <= 0){
		fprintf(stderr, "Bad number of router\n");
		return 1;
	}
	
	// Vytvoreni zpravy
	remote_request.request = remote_close_conn;
	
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
	
	return remote_response.request != remote_response_ok;		
}
