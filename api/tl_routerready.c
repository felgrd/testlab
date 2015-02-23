#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>    // atoi
#include <errno.h>     // errno
#include <unistd.h>    // getopt
#include <string.h>    // strdup
#include <time.h>      // time
#include <sys/types.h> // waitpid
#include <sys/wait.h>  // waitpid
#include "utils.h"
#include "cspipe.h"

// Defaultni cas zkouseni stavu routeru
#define DEFUALT_TIMEOUT		120

/**
 * @file tl_routerready.c
 *
 * @brief usage tl_routerready [-t <timeout>] [-i <ip> || <id>]
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 29.1.2015
 *
 * Program tl_ready wait for router run and answer seconds of wait. <br>
 * Example command: tl_routerready 1. Answer: 20. Program wait 20 seconds.
 *
 * @param -t <timeout> Maximal time for try ping to the router.
 * @param -i <ip> IP address of router.
 * @param <id> ID of router.
 *
 * @return 0 - Answer is valid.<br>
 *         1 - Parameter is not valid.<br>
 *         2 - Timeout is expired.
 *
 * @cond
 */

#define STATE_OFF      0
#define STATE_PING     1
#define STATE_CONNECT  2
#define STATE_TIMEOUT  3
#define STATE_ERROR    4

void help(void){
	printf("usage tl_routerready [-t <timeout>] [-i <ip> || <id>]\n");
}

int main(int argc, char *argv[]){
	int     timeout;            // Cas maximalniho cekani na odpoved routeru
	int     starttime;          // Cas spusteni programu
	int     state;              // Stav testovani
	char    ip[30];             // IP adresa testovaneho routeru
	int     router;             // Identifikator routeru
	int     status;             // Navratovy status ukonceneho procesu
	int     parameter;          // Parametr prichayejici na prikazove radcec
	pid_t   pid;                // pid spusteneho proces
	pid_t   wpid;               // pid ukonceneho procesu
  int     result;
	char    request[PIPE_BUFFER_SIZE];    // Buffer pro opoved z routeru
	char    response[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru

  // Inicializace statavu a promenych
	state = STATE_OFF;
	timeout = DEFUALT_TIMEOUT;
	ip[0] = '\0';
	router = 0;

	// Rozebirani parametru na prikazove radce
	while ((parameter = getopt(argc, argv, "i:t:")) != -1) {
		switch (parameter) {
			case 'i':
				strncpy(ip, optarg, sizeof(ip));
				break;
			case 't':
				timeout = atoi(optarg);
				if(timeout <= 0){
					timeout = DEFUALT_TIMEOUT;
				}
				break;
			case '\?':
				help();
				return 1;
			case ':':
				help();
				return 1;
		}
	}

  // Zjisteni ip adresy z identifikatoru routeru
	if(strlen(ip) == 0){
		// Kontrola parametru id routeru
		if(optind >= argc){
			help();
			return 1;
		}

		// Kontola platneho hodnoty parametru
		router = atoi(argv[optind]);
		if(router <= 0){
			help();
			return 1;
		}

		// Zjisteni IP adresy routeru podle identifikatoru
		result = pipe_request(router, remote_status_address, NULL, ip);
		if(!result){
			return 1;
		}
	}

	// Zacatek kontroly stavu routeru
	starttime = time(NULL);

	// Ping na router
	do{

		switch(pid = vfork()){

		case -1:
			fprintf(stderr, "Create new process for checkout error (%d).", errno);
			return 1;
			break;

		case 0:
			// Potlaceni vsech vystupu
			close_all_fds(-1);
			// Spusteni pingu
			execlp("ping", "ping", "-c 1", ip, NULL);
			break;

		default:
			// Cekani na ukonceni pingu
			wpid = waitpid(pid, &status, 0);

			// Kontrola korektniho ukonceni skriptu
			if(wpid != pid){
				state = STATE_ERROR;
			}

			// Kontrola timeoutu programu
			if(starttime + timeout < time(NULL)){
				state = STATE_TIMEOUT;
				fprintf(stderr, "Timeout expired while ping to router.\n");
			}

			// Zjisteni uspesnosti pingu
			if(!WEXITSTATUS(status)){
				state = STATE_PING;
			}

			break;
		}

	}while(!state);

	// Cekani na odpoved od routeru
	while(state < STATE_CONNECT){

		// Vytvoreni zpravy
		snprintf(request, sizeof(request), "echo connection");

		// Odeslani zadosti remote serveru
		result = pipe_request(router, remote_process, request, response);

		// Kontrola timeoutu
		if(starttime + timeout < time(NULL)){
			state = STATE_TIMEOUT;
			fprintf(stderr, "Timeout expired while connect to router.\n");
		}

		// Kontrola odpovedi od routeru
		if(strcmp(response, "connection") == 0){
			state = STATE_CONNECT;
		}
	}

	// Tisk celkove doby cekani na nabehnuti routeru
	printf("%d\n", (int)(time(NULL) - starttime));

	return state == STATE_CONNECT ? 0 : 2;
}
