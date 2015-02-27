#include <sys/types.h>	// pipe
#include <stdlib.h>     // atoi
#include <stdio.h>      // printf
#include <string.h>     // strncpy
#include "cspipe.h"

/**
 * @file tl_remote.c
 *
 * @brief usage tl_remote \<id\> \<command\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 29.1.2015
 *
 * Program tl_remote execute command on tested router. <br>
 * Example command: tl_remote 4 "io get bin0". Ansewer: State of binary input 0.
 *
 * @param <id> Router ID of your tested router.
 * @param <command> The command that will be execute in the router.
 *
 * @returns 0-119 - Command is execute with same return value.<br>
 *          120   - Commnad is not execute.
 *
 * @cond
 */

void help(void){
	fprintf(stderr, "usage tl_remote <id> <command>\n");
}

int main(int argc, char *argv[]){
	int     router;                     // Identifikator routeru
	int     result;                     // Navratovy kod funkci
	char    answer[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru

	// Kontrola parametru command
	if(argc != 3){
		help();
		return 120;
	}

	// Nazev serverove roury
	router = atoi(argv[1]);
	if(router <= 0){
		help();
		return 120;
	}

	// Odeslani zadosti remote serveru
	result = pipe_request(router, remote_process, argv[2], answer);

	// Kontrola odpovedi
	if(result < 0){
		fprintf(stderr, "%s\n", answer);
		return 120;
	}

	// Tisk korektni odpovedi
	printf("%s\n", answer);

	// Ukonceni s navratovou hodnotou z remote serveru
	return result;
}
