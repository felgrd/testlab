#define _XOPEN_SOURCE 500
#include <stdio.h>		// snprintf
#include <stdlib.h>     // atoi
#include <unistd.h>		// chdir
#include <syslog.h>		// syslog
#include <errno.h>    // errno
#include <unistd.h>   // chdir
#include <sys/wait.h> // waitpid
#include <sys/types.h> // open
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "utils.h"
#include "database.h"

/**
  * Program for clean project
  *
  * @param release_id		is id of acual release in database
  * @param platform_id	is name of checkout project
  * @param platform_name	is name of checkout project
*/

int main(int argc, char *argv[]){
	int     release_id;
	int     platform_id;
	char    *platform_name;
	pid_t   pid;
	pid_t   wpid;
	int     result;              // Navratovy kod
	int     status;
	char    command[50];         // Buffer pro prikaz
	int     timeout;             // Timeout pro dokonceni skriptu
	int     waittime;            // Doba behu skriptu
	int     log_fd;              // File descriptor pro soubor s logem
	char    log_name[PATH_MAX];  // Nazev souboru s logem

	// Otevreni logu
	openlog("TestLabCLear", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE, "Start clear project %s", argv[3]);

	// Inicializace promenych
	waittime = 0;

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

	// Zjisteni timeoutu pro checkout script
	timeout = database_sel_timeout(STATE_CLEAN);
	if(timeout <= 0){
		syslog(LOG_ERR, "Timeout for clear script does not read %d.", timeout);
		return 1;
	}

	// Sestaveni cesty k souboru s logem
	snprintf(log_name, sizeof(log_name), "%s/logs/clean/%d/%d.log", \
	DIRECTORY, release_id, platform_id);

	// Otevreni souboru s logem
	log_fd = open(log_name, O_CREAT|O_RDWR, 0666);
	if(log_fd < 0){
		syslog(LOG_ERR, "Open log file error (%d)", errno);
		return 1;
	}

	switch(pid = fork()){
		case -1:
			syslog(LOG_ERR, "Create new process for clean error (%d).", errno);
			return 0;

		case 0:
			// Uzavreni logu
			closelog();

			// Uzavreni standartniho vystupu a presmerovani do log souboru
			close(1);
			dup(log_fd);

			// Uzavreni chyboveho vystupu a presmerovani do log souboru
			close(2);
			dup(log_fd);

			// Spusteni skriptu
			execlp("bash", "bash", command, NULL);

			// Ukonceni programu v pripade chyby
			return 1;

		default:
			// Timeout ukonceni uzivatelskeho scriptu
			do{
				// Kontrola stavu skriptu
				wpid = waitpid(pid, &status, WNOHANG);

				// Kontrola ukonceni skriptu
				if(wpid == 0){
					if(waittime < timeout){
						waittime++;
						sleep(1);
					}else{
						kill(pid, SIGKILL);
					}
				}

			}while(wpid == 0 && waittime <= timeout);

			// Kontrola spravne ukonceneho procesu
			if(wpid == -1){
				return 1;
			}

			break;
	}

	closelog();

	return 0;
}
