#define _XOPEN_SOURCE 500
#include <stdio.h>		// snprintf
#include <syslog.h>		// syslog
#include <stdlib.h>     // atoi
#include <errno.h>		// errno
#include <unistd.h>		// chdir
#include <sys/wait.h>	// waitpid 
#include <limits.h>

#include "database.h"
#include "utils.h"


 /**
    * Program for compile project
    * 
    * @param release_id		is id of acual release in database
    * @param platform_id	is name of checkout project
    * @param platform_name	is name of checkout project
	*/

int main(int argc, char *argv[]){
	int		release_id;				// ID aktualniho releasu v databazi		
	int		platform_id;			// ID platformy
	int		product_id;				// ID produktu
	char	*platform_name;			// Nazev platformy	
	int		result;					// Navratovy kod funkci
	char	command[50];			// Buffer pro sestaveni prikazu
	char	directory[PATH_MAX];	// Adresar se zkompilovanym fw
	char	***products;			// Pole pro ulozeni vsech produktu
	int 	platform_build;			// Vysledek prekladu platformy
	int 	status;					// Navratovy status ukonceneho procesu
	pid_t	pid;					// PID vytvoreneho procesu
	pid_t	wpid;					// PID ukonceneho procesu
	int		i;						// Promena pro prochazeni pole
		
	// Otevreni logu
	openlog("TestLabCompile", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	
	// Inicializace promenych
	platform_build = 1;
	
	// Kontrola poctu parametru
	if(argc != 4){
		syslog(LOG_ERR, "Bad number of parameters. " \
		" Usage: checkout <release> <platform_id> <platform_name>.");
		return 1;
	}
	
	// Parametr release ID
	release_id = atoi(argv[1]);
	if(release_id <= 0){
		syslog(LOG_ERR, "Number release id error.");
		return 1;
	}
	
	// Parametr platform ID
	platform_id = atoi(argv[2]);
	if(platform_id <= 0){
		syslog(LOG_ERR, "Number project id error.");
		return 1;
	}
	
	// Parametr platform name
	platform_name = argv[3];
	
	// Adresar projektu
	result = snprintf(command, sizeof(command), "%s/project/%d/%s", \
	DIRECTORY, release_id, platform_name);
	if(result < 3){
		return 1;
	}
	
	// Zmena pracovniho adresar
	result = chdir(command);
	if(result < 0){
		syslog(LOG_ERR, "Change directory to %s error (%d).", \
		command, errno);
		return 1;
	}
	
	// Ziskani vsech produktu z databaze
	products = database_sel_products(platform_id);
	if(products == NULL){
		syslog(LOG_ERR, "Database error: select products.");
		return 1;
	}
	
	// Sestaveni prikazu pro kompilaci platformy
	snprintf(command, sizeof(command), "%s/compile/%s", DIRECTORY, \
	platform_name);
	
	// Kontrola existence scriptu
	if(access(command, F_OK) == -1){
		syslog(LOG_ERR, "Script does not exist. (%s)", command);
		return 1;
	}
	
	// Adresar se zkompilovanym fw
	snprintf(directory, sizeof(directory), "%s/firmware/%d", DIRECTORY, \
	release_id);
	
	// Vytvoreni adresare pro zkompilovany fw
	mkdir(directory, S_IRWXU);
	if(result < 0){
		syslog(LOG_ERR, "Create directory error (%d)", errno);
		return 1;
	}
	
	// Kontrola existence scriptu
	
	
	// Prochazeni vsemi produktami
	for(i = 0; products[i] != NULL; i++){
		
		// Prevod id produktu na cislo
		product_id = atoi(products[i][DB_PRODUCTS_ID]);
		if(product_id <= 0){
			return 1;
		}
		
		// Vykonani checkout scriptu
		switch (pid = vfork()) {
			case -1:
				syslog(LOG_ERR, "Create new process for compile error" \
				" (%d).", errno);
				return 1;
			case 0:
				// Potlaceni vsech vystupu
				close_all_fds(-1);
		
				// Spusteni skriptu			
				execlp("bash", "bash", command, directory, \
				products[i][DB_PRODUCTS_NAME], NULL);
	
				// Ukonceni programu v pripade chyby
				return 1;
			default:
		
				// Timeout ukonceni uzivatelskeho scriptu
		
				
				// Cekani na dobehnuti scriptu
				wpid = waitpid(pid, &status, 0);		
			
				// Kontrola spravne ukonceneho procesu
				if(wpid == -1){
					return 1;
				}	
			
				// Vlozeni vysledku preklad produktu do databaze
				database_ins_build_product(release_id, product_id, \
				!WEXITSTATUS(status));
			
				// Vysledek prekladu platformy
				platform_build &= !WEXITSTATUS(status);
			
				break;
		}		
	}	
	
	// Vlozeni vysledku preklad platformy do databaze
	database_ins_build_platform(release_id, platform_id, platform_build);
	
	// Uvolneni pameti
	database_result_free(products);	
	
	// Uzavreni logu
	closelog();
	
	return 0;
}
