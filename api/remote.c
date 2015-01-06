#include <sys/types.h>		// pipe
#include <stdlib.h>			// atoi
#include <stdio.h>			// printf
#include <string.h>
#include "cspipe.h"


 /**
    * Program for remote
    * 
    * @param router		ID of router
	*/

int main(int argc, char *argv[]){
	int		fifo_fd;						// Roura pro prijem dat
	int		router;
	int		result;
	message_remote remote_request;			// Format posilanych dat
	message_remote remote_response;			// Format posilanych dat
		
	// Kontrola parametru command
	if(argc != 3){
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
