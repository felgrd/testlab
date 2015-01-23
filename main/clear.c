#define _XOPEN_SOURCE 500
#include <stdio.h>		// snprintf
#include <stdlib.h>     // atoi
#include <unistd.h>		// chdir
#include <syslog.h>		// syslog
#include "utils.h"

 /**
    * Program for clean project
    *
    * @param release_id		is id of acual release in database
    * @param platform_id	is name of checkout project
    * @param platform_name	is name of checkout project
	*/

int main(int argc, char *argv[]){
	int		release_id;
	int		platform_id;
	char	*platform_name;
	int		result;				// Navratovy kod
	char	command[50];		// Buffer pro prikaz

	// Otevreni logu
	openlog("TestLabCLear", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	// Kontrola poctu parametru
	if(argc != 4){
		syslog(LOG_ERR, "Bad number of parameters. " \
		"Usage: clear <release> <platform_id> <platform_name>.");
		return 1;
	}

	// Parametr release ID
	release_id = atoi(argv[1]);
	if(release_id <= 0){
		syslog(LOG_ERR, "Number release id error.");
		return 1;
	}

	// Parametr project ID
	platform_id = atoi(argv[2]);
	if(platform_id <= 0){
		syslog(LOG_ERR, "Number project id error.");
		return 1;
	}

	// Parametr project name
	platform_name = argv[3];

	// Adresar pro stazeni projektu
	result = snprintf(command, sizeof(command), "%s/project/%d" \
	"/%s", DIRECTORY, release_id, platform_name);
	if(result < 3){
		return 1;
	}

	// Zmena pracovniho adresare
	result = chdir(command);
	if(result < 0){
		return 1;
	}

	// Sestaveni prikazu
	result = snprintf(command, sizeof(command), "%s/clean/%s", \
	DIRECTORY, platform_name);
	if(result < 2){
		return 1;
	}

	// Kontrola existence scriptu
	if(access(command, F_OK) == -1){
		syslog(LOG_ERR, "Script does not exist. (%s)", command);
		return 1;
	}

	// Potlaceni vsech vystupu
	close_all_fds(-1);

	// Spusteni skriptu
	execlp("bash", "bash", command, NULL);

	syslog(LOG_ERR, "Script no running.");

	return 1;
}
