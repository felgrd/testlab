#define _XOPEN_SOURCE 500
#include <stdio.h>       // snprintf
#include <syslog.h>      // syslog
#include <stdlib.h>      // atoi
#include <errno.h>       // errno
#include <unistd.h>      // chdir
#include <sys/wait.h>    // waitpid
#include <sys/select.h>  // select
#include <string.h>      // memcpy
#include <limits.h>
#include <time.h>        // time
#include "database.h"
#include "utils.h"

/**
  * Program for test router
  *
  * @param release_id		is id of acual release in database
  * @param router_id		is name of checkout project
  * @param router_name	is name of checkout project
*/

int main(int argc, char *argv[]){
	int     release_id;          // ID testovaneho releasu v databazi
	char    *release;            // ID testovaneho releasu v databazi
	int     router_id;           // ID testovaneho routeru v databazi
	char    *router;             // ID testovaneho routeru v databazi
	int     function_id;         // ID testovane funkce v databazi
	int     procedure_id;        // ID testovane procedury v databazi
	int     test_router;         // Vysledek testu testovaneho routeru
	int     test_function;       // Vysledek testu testovane funkce
	int     result;              // Navratovy kod funkce
	char    ***functions;        // Funkce testovaneho routeru
	char    ***procedures;       // Procedury testovane funkce
	char    command[PATH_MAX];   // Buffer pro sestaveni commandu
	char    buffer_std[256];     // Buffer na data z std vystupu
	char    buffer_err[256];     // Buffer na data z err vystupu
	char    test_value[512];     // Hodnota testu
	int     size_std;            // Velikost dat z std vystupu
	int     size_err;            // Velikost dat z err vystupu
	int     file_pipes_std[2];   // Roura pro std vystup scriptu
	int     file_pipes_err[2];   // Roura pro err vystup scriptu
	fd_set  read_fd_set;         // Cekani na prichod dat
	pid_t   pid;                 // PID vytvoreneho procesu
	pid_t   wpid;                // PID ukonceneho procesu
	int     status;              // Status ukonceneho procesu
	int     i, j;                // Promene pro prochazeni poli
	int     timeout;             // Timeout pro dokonceni skriptu
	int     starttime;           // Cas startu skriptu

	// Otevreni logu
	openlog("TestLabTest", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	// Inicializace promenych
	test_router = 1;

	// Kontrola poctu parametru
	if(argc != 3){
		syslog(LOG_ERR, "Bad number of parameters. " \
		" Usage: tl_test <release_id> <router_id>.");
		return 1;
	}

	// Parametr release ID
	release = argv[1];
	release_id = atoi(release);
	if(release_id <= 0){
		syslog(LOG_ERR, "Number release id error.");
		return 1;
	}

	// Parametr router ID
	router = argv[2];
	router_id = atoi(router);
	if(router_id <= 0){
		syslog(LOG_ERR, "Number router id error.");
		return 1;
	}

	// Ziskani funkci z databaze
	functions = database_sel_functions(router_id);
	if(functions == NULL){
		syslog(LOG_ERR, "Database error: select functions.");
		return 1;
	}

	// Zjisteni timeoutu pro test script
	timeout = database_sel_timeout(STATE_TESTROUTER);
	if(timeout <= 0){
		syslog(LOG_ERR, "Timeout for test script does not read.");
		return 1;
	}

	// Prochazeni vsech funkci
	for(i = 0; functions[i] != NULL; i++){

		// Inicializace vysledku testu funkce
		test_function = 1;

		// Prevod id funkce na cislo
		function_id = atoi(functions[i][DB_FUNCTIONS_ID]);
		if(function_id <= 0){
			syslog(LOG_ERR, "Functions ID is not number.");
			continue;
		}

		// Ziskani procedur z databaze
		procedures = database_sel_procedures(function_id);
		if(procedures == NULL){
			syslog(LOG_ERR, "Database error: select procedures.");
			continue;
		}

		// Prochazeni procedur dane funkce
		for(j = 0; procedures[j] != NULL; j++){

			// Prevod id procedury na cislo
			procedure_id = atoi(procedures[j][DB_PROCEDURES_ID]);
			if(procedure_id <= 0){
				syslog(LOG_ERR, "Procedure id is not number.");
				continue;
			}

			// Sestaveni commandu
			result = snprintf(command, sizeof(command), \
			"%s/tests/%s/%s.tst", DIRECTORY, \
			functions[i][DB_FUNCTIONS_NAME], \
			procedures[j][DB_PRODUCTS_NAME]);

			// Kontrola sestaveni commandu
			if(result < 4){
				syslog(LOG_ERR, "Test script name does not create.");
				continue;
			}

			// Kontrola existence scriptu
			if(access(command, F_OK) == -1){
				syslog(LOG_ERR, "Script does not exist. (%s)", command);
				continue;
			}

			// Vytvoreni roury pro predavani hodnot
			if(pipe(file_pipes_std) == -1){
				syslog(LOG_ERR, "Create pipe std error (%d)", errno);
				return 1;
			}

			// Vytvoreni roury pro predavani chyb
			if(pipe(file_pipes_err) == -1){
				syslog(LOG_ERR, "Create pipe err error (%d)", errno);
				return 1;
			}

			// Vykonani testovaciho scriptu
			switch (pid = vfork()) {
			case -1:
				syslog(LOG_ERR, "Create new process error (%d).", errno);
				return 1;

			case 0:
				// Nastaveni vstupu a vystupu
				close(1);
				dup(file_pipes_std[1]);
				close(2);
				dup(file_pipes_err[1]);
				close(file_pipes_std[0]);
				close(file_pipes_std[1]);
				close(file_pipes_err[0]);
				close(file_pipes_err[1]);

				// Spusteni scriptu
				execlp("bash", "bash", command, router, release, NULL);

				syslog(LOG_ERR, "Test script no running. (%d)", errno);

				return 1;

			default:

				// Nastavani casu startu skriptu
				starttime = time(NULL);

				// Nastaveni vstupu a vystupu
				close(file_pipes_std[1]);
				close(file_pipes_err[1]);

				// Cekani na konec scriptu a kontrolovani vystupu
				do{

					// Cekani na udalost
					FD_ZERO(&read_fd_set);
					FD_SET(file_pipes_std[0], &read_fd_set);
					FD_SET(file_pipes_err[0], &read_fd_set);
					if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, \
					NULL) >= 0) {

						// Prijem dat ze standartniho vystupu scriptu
						if(FD_ISSET(file_pipes_std[0], &read_fd_set)){

							// Cteni dat
							size_std = read(file_pipes_std[0], \
							buffer_std, sizeof(buffer_std));

							// Prijem platnych dat
							if(size_std > 0){
								buffer_std[size_std] = '\0';
								memcpy(buffer_std, test_value, size_std);
							}
						}

						// Prijem dat z chyboveho vystupu scriptu
						if(FD_ISSET(file_pipes_err[0], &read_fd_set)){

							// Cteni dat
							size_err = read(file_pipes_err[0], \
							buffer_err, sizeof(buffer_err));

							// Prijem platnych dat
							if(size_err > 0){
								buffer_err[size_err] = '\0';

								// Vlozeni chyby do databazeho logu
								database_ins_log(procedure_id, router_id, \
								release_id, buffer_err);
							}
						}
					}

					// Kontrola vyprseni timeoutu skriptu
					if(time(NULL) - starttime > timeout){
						kill(pid, SIGKILL);
					}

					// Cekani na dobehnuti scriptu
					wpid = waitpid(pid, &status, WNOHANG);

					// Kontrola chyby pri cekani na proces
					if(wpid == -1){
						syslog(LOG_ERR, "Termination process error.");
						return 1;
					}

				}while(wpid == 0);

				// Ulozeni vysledku procedury do databaze
				database_ins_res_procedure(procedure_id, router_id, \
				release_id, !WEXITSTATUS(status), test_value);

				// Uprava vysledku testu funkce
				test_function &= !WEXITSTATUS(status);

				// Uprava vysledku testu routeru
				test_router &= !WEXITSTATUS(status);

				// Uzavreni deskriptoru
				close(file_pipes_std[0]);
				close(file_pipes_err[0]);

				break;
			}
		}

		// Uvolneni pameti
		if(procedures != NULL){
			database_result_free(procedures);
			procedures = NULL;
		}

		// Vlozeni vysledku testu funkce do databaze
		database_ins_res_function(function_id, router_id, release_id, \
		test_function);
	}

	// Vlozeni vysledku testu routeru do databaze
	database_ins_res_router(router_id, release_id, test_router);

	// Uvolneni pameti
	if(functions != NULL){
		database_result_free(functions);
		functions = NULL;
	}

	// Zavreni logu
	closelog();

	return 0;
}
