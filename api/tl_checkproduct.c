#include <stdio.h>
#include <stdlib.h>
#include "database.h"

/**
 * @file tl_checkproduct.c
 *
 * @brief usage tl_checkproduct \<id\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 29.1.2015
 *
 * Program returns firmware name of tested router. <br>
 * Example command: tl_checkproduct 4. Answer: LR77-v2.
 *
 * @param <id> Router ID of your tested router.
 *
 * @return 0 - Name of router firmware is valid<br>
 *         1 - Name of router firmware is not valid
 *
 * @cond
 */

void help(void){
	fprintf(stderr, "usage tl_checkproduct <id>\n");
}

int main(int argc, char *argv[]){
	int		  router;             //
	char	  *product;           //

	// Kontrola poctu parametru
	if(argc != 2){
		return 1;
	}

	// Kontrola parametru id routeru
	router = atoi(argv[1]);
	if(router <= 0){
		return 1;
	}

	// Provedeni SQL dotazu
	product = database_sel_product(router);

	// Kontrola vysledku dotazu
	if(product == NULL){
		return 1;
	}

	// Tisk vysledku
	printf("%s", product);

	// Uvolneni pameti
	free(product);

	return 0;
}
