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
 * @date 29.1.2015
 *
 * Program send status command to the router and return answer.<br>
 * Example command: tl_status Mobile Technology. Answer: UMTS.
 *
 * @param <id> Router id of your tested router.
 * @param <category> Category of router status.
 * @param <subcategory> Item of router status.
 *
 * @return 0 -> Answer is valid.<br>
 *         1 -> Parameter is not valid.<br>
 *         2 -> Wrong connection to the router.<br>
 *         3 -> Bad answer from the router.
 *
 * @cond
 */

void help(void){
	fprintf(stderr, "usage tl_status <id> <category> [<subcategory>]\n");
}

int main(int argc, char *argv[]){
	int     router;                       // Identifikator routeru
	int     result;                       // Navratovy kod funkce
	char    *category;                    // Dotazovana kategorie statusu
	char    *subcategory;                 // Dotazovana polozaka statusu
	char    *find_start;                  // Ukazatel pro vyhledavani
	char    *find_stop;                   // Ukazatel pro vyhledavani
	char    value[512];                   // Buffer pro ulozeni hledaneho vysledku
	char    request[PIPE_BUFFER_SIZE];    // Buffer pro opoved z routeru
	char    response[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru

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
	snprintf(request, sizeof(request), "status -v %s", category);

	// Odeslani zadosti remote serveru
	result = pipe_request(router, remote_process, request, response);

	// Kontrola odpovedi
	if(!result){
		return 2;
	}

	// Inicializace vysledku
	result = 3;

	// Ziskani vysledku
	if(subcategory == NULL){
		// Tisk kategorie
		printf("%s", response);

		// Uprava vysledku
		result = 0;
	}else{
		// Zjisteni subkategorie
		find_start = strstr(response, subcategory);

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

	return result;
}
