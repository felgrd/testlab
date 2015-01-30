#define _XOPEN_SOURCE 500
#include <stdio.h>    // snprintf
#include <stdlib.h>   // atoi
#include <syslog.h>   // syslog
#include <errno.h>    // errno
#include <unistd.h>   // chdir
#include <sys/wait.h> // waitpid
#include "database.h"
#include "utils.h"

 /**
    * Program for checkout project from repository
    *
    * @param release_id		is id of acual release in database
    * @param project_id		is name of checkout project
    * @param project_name	is name of checkout project
	*/

int main(int argc, char *argv[]){
	int    result;           // Navratovy kod funkci
	char   command[50];      // Buffer pro prikazy
	pid_t  pid;              // Pid vytvoreneho procesu
	pid_t  wpid;             // Pid cekaneho procesu
	int    status;           // Navratovy status ukonceneho procesu
	int    release_id;       // ID aktualniho releasu v databazi
	int    platform_id;      // ID platformy
	char   *platform_name;   // Nazev platformy
	int    timeout;          // Timeout pro dokonceni skriptu
	int    waittime;         // Doba behu skriptu

	// Otevreni logu
	openlog("TestLabCheckout", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	// Inicializace promenych
	waittime = 0;

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

	// Vytvoreni adresare pro stazeni project
	result = mkdir(command, S_IRWXU);
	if(result < 0){
		syslog(LOG_ERR, "Create directory error (%d).", errno);
		return 1;
	}

	// Zmena pracovniho adresare
	result = chdir(command);
	if(result < 0){
		syslog(LOG_ERR, "Change directory to %s error (%d).", \
		command, errno);
		return 1;
	}

	// Sestaveni prikazu
	result = snprintf(command, sizeof(command), "%s/checkout/%s", \
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
	timeout = database_sel_timeout(STATE_CHECKOUT);
	if(timeout <= 0){
		syslog(LOG_ERR, "Timeout for checkou script does not read.");
		return 1;
	}

	// Vykonani checkout scriptu
	switch(pid = vfork()) {
		case -1:
			syslog(LOG_ERR, "Create new process for checkout error" \
			" (%d).", errno);
			return 0;

		case 0:
			// Potlaceni vsech vystupu
			close_all_fds(-1);

			// Spusteni skriptu
			execlp("bash", "bash", "-f", command, NULL);

			// Ukonceni programu v pripade chyby
			return 1;

		default:
			// Timeout ukonceni uzivatelskeho scriptu
			do{
				// Kontrala stavu skriptu
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

			// Vlozeni vysledku preklad produktu do databaze
			database_ins_checkout(release_id, platform_id, \
			!WEXITSTATUS(status));

			break;
	}

	// Uzavreni logu
	closelog();

	return 0;
}
