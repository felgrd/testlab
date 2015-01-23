#include <stdio.h>    // fprintf
#include <stdlib.h>   // atoi
#include <string.h>   // strncpy
#include "cspipe.h"

/**
 * @file tl_status.c
 *
 * @brief usage tl_status \<id\> \<category\> [\<subcategory\>]
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 6.1.2015
 *
 * Program send status command to the router and return answer.<br>
 * Example command: tl_status Mobile Technology. Answer: UMTS.
 *
 * @param <id> Router id of your tested router.
 * @param <category> Category of router status.
 * @param <subcategory> Subcategory of router status.
 *
 * @return 0 -> Answer is valid.<br>
 *         1 -> Parameter is not valid.<br>
 *         2 -> Wrong connection to the router.<br>
 *         3 -> Bad answer from the router.
 *
 * @cond
 */

void help(void){
	fprintf(stderr, "usage tl_status <id> <category> [<subcategory>]");
}

int main(int argc, char *argv[]){
	int		fifo_fd;                  // Roura pro prijem dat
	int		router;
	int		result;
	char	*category;
	char	*subcategory;
	char	*find_start;
	char	*find_stop;
	char	value[512];
	message_remote remote_request;   // Format posilanych dat
	message_remote remote_response;  // Format posilanych dat

	// Kontrola poctu parametru
	if(argc < 3){
		help();
		return 1;
	}

	// Parametr nazev serverove roury
	router = atoi(argv[1]);
	if(router <= 0){
		help();
		return 1;
	}

	// Parametr kategorie statusu
	category = argv[2];

	// Parametr subkategorie statusu
	if(argc == 3){
		subcategory = NULL;
	}else{
		subcategory = argv[3];
	}

	// Vytvoreni zpravy
	snprintf(remote_request.data, sizeof(remote_request.data), \
	"status -v %s", category);
	remote_request.request = remote_process;

	// Navazani spojeni
	fifo_fd = client_starting(router);
	if(!fifo_fd){
		return 2;
	}

	// Odelani zadosti
	result = send_mess_to_server(fifo_fd, remote_request);
	if(!result){
		client_ending(router, fifo_fd);
		return 2;
	}

	// Prijem odpovedi
	result = read_response_from_server(&remote_response);
	if(!result){
		client_ending(router, fifo_fd);
		return 2;
	}

	// Kontrola odpovedi
	if(remote_response.request != remote_response_ok){
		client_ending(router, fifo_fd);
		printf("%s\n", remote_response.data);
		return 2;
	}

	// Inicializace vysledku
	result = 3;

	// Ziskani vysledku
	if(subcategory == NULL){
		// Tisk kategorie
		printf("%s", remote_response.data);

		// Uprava vysledku
		result = 0;
	}else{
		// Zjisteni subkategorie
		find_start = strstr(remote_response.data, subcategory);

		if(find_start != NULL){
			// Hledani zacatku hodnoty
			find_start = strchr(find_start, ':') + 2;

			if(find_start != NULL){
				// Hledani konce radku - odriznuti /r/n
				find_stop = strchr(find_start, '\n') -1;

				if(find_start != NULL){
					// Kopirovani a tisk hodnoty
					strncpy(value, find_start, find_stop - find_start);
					printf("%s", value);

					// Uprava vysledku
					result = 0;
				}
			}
		}
	}

	// Ukonceni komunikace
	client_ending(router, fifo_fd);

	return result;
}
