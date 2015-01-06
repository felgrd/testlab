#include <stdio.h>
#include <stdlib.h>
#include "database.h"


int main(int argc, char *argv[]){
	int		router;
	char	*product;
	
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
	
	return 0;
}
