#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <time.h>      // time, localtime
#include <libgen.h>    // dirname
#include <unistd.h>    // getlogin
#include <sys/stat.h>  // mkdir
#include <sys/wait.h>  // waitpid
#include <string.h>    // strncpy
#include <errno.h>     // errno
#include <syslog.h>    // syslog
#include <stdlib.h>    // malloc, free
#include <limits.h>
#include "database.h"
#include "utils.h"


int main(int argc, char *argv[])
{
	time_t        actual_time;          // Aktualni cas
	struct tm     tm;                   // Aktualni cas v jednotkach casu
	char          *release_type;        // Druh releasu firmwaru
	int           result;               // Navratovy kod funkce
	char          date[50];             // Aktualni cas jako retezec
	long long int release_id;           // Id vlozeneho releasu
	char          release[50];          // Id vlozeneho releasu v retezci
	char          ***platforms;         // Pole tabulky platforms
	char          ***routers;           // Pole tabulkty routers
	int           i;                    // Promene na prochazeni poli
	pid_t         pid;                  // Pid vytvoreneho procesu
	int           childs;               // Pocet vytvorenych procesu
	int           status;               // Status ukonceneho procesu
	int           childs_remote;        // Pocet procesu remote
	char          directory[PATH_MAX];  // Cesta k adresari

	/******************************************************************
	 *  Zpracovani parametru
	 *****************************************************************/

	// Otevreni syslogu
	openlog ("TestLab", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE, "Program started by User %d", getuid ());

	// Zpracovani parametru release fimwaru
	if(argc != 2){
		syslog(LOG_ERR, "Bad parameters. Usage: test <release>.");
		return 1;
	}
	release_type = argv[1];

	/******************************************************************
	*  Priprava testu
	*****************************************************************/

	// Datum a cas vydani firmwaru
	actual_time = time(NULL);
	tm = *localtime(&actual_time);
	snprintf(date, sizeof(date), "%d-%d-%d %d:%d:%d", tm.tm_year + \
	1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	// Vlozeni aktualniho releasu do databaze
	release_id = database_ins_release(date, release_type);
	if(release_id == 0){
		syslog(LOG_ERR, "Database error: release inserted.");
		return 1;
	}

	sprintf(release, "%lld", release_id);

	// Vytvoreni pracovniho adresare
	snprintf(directory, sizeof(directory), "%s/project/%lld", DIRECTORY, release_id);
	result = mkdir(directory, S_IRWXU);
	if(result < 0){
		if(errno == EEXIST){
			syslog(LOG_ERR, "Directory project is exist.");
		}else{
			syslog(LOG_ERR, "Create project directory error (%d).", errno);
		}
		return 1;
	}

	/******************************************************************
	 *  Stahnuti projektu z repozitaru
	 *****************************************************************/

	// Adresar pro logy checkoutu
	snprintf(directory, sizeof(directory), "%s/logs/checkout/%lld", DIRECTORY, \
	release_id);

	// Vytvoreni adresare pro logy checkoutu
	mkdir(directory, S_IRWXU);
	if(result < 0){
		syslog(LOG_ERR, "Create logs directory error (%d)", errno);
		return 1;
	}

	// Ziskani vsech platforem z databaze
	platforms = database_sel_platforms();
	if(platforms == NULL){
		syslog(LOG_ERR, "Database error: select platform.");
		return 1;
	}

	// Inicializace poctu potomku
	childs = 0;

	// Prochazeni vsemi platformami
	for(i = 0; platforms[i] != NULL; i++){

		// Vytvoreni procesu pro stazeni projektu
		switch (pid = vfork()) {
			case -1:
				syslog(LOG_ERR, "Create new process for checkout error (%d).", errno);
				return 0;
			case 0:
				// Potlaceni vsech vystupu
				close_all_fds(-1);

				// Spusteni skriptu
				execlp("tl_checkout", "tl_checkout", release, \
				platforms[i][DB_PLATFORMS_ID], \
				platforms[i][DB_PLATFORMS_NAME], NULL);

				// Ukonceni programu v pripade chyby
				return 1;
			default:
				// Pocet bezicich potomku
				++childs;
				break;
		}
	}

	// Cekani na ukonceni vsech procesu tl_checkout
	while(childs--){
		// Cekani na ukonceni procesu
		pid =  wait(&status);
	}

	/******************************************************************
	 *  Kompilace projektu
	 *****************************************************************/

	// Adresar pro logy kompilace
	snprintf(directory, sizeof(directory), "%s/logs/compile/%lld", DIRECTORY, \
	release_id);

	// Vytvoreni adresare pro logy kompilace fw
	mkdir(directory, S_IRWXU);
	if(result < 0){
		syslog(LOG_ERR, "Create logs directory error (%d)", errno);
		return 1;
	}

	// Adresar se zkompilovanym fw
	snprintf(directory, sizeof(directory), "%s/firmware/%lld", DIRECTORY, \
	release_id);

	// Vytvoreni adresare pro zkompilovany fw
	mkdir(directory, S_IRWXU);
	if(result < 0){
		syslog(LOG_ERR, "Create fw directory error (%d)", errno);
		return 1;
	}

	// Inicializace poctu potomku
	childs = 0;

	// Prochazeni vsemi platformami
	for(i = 0; platforms[i] != NULL; i++){

		// Vytvoreni procesu pro stazeni projektu
		switch (pid = vfork()) {
			case -1:
				syslog(LOG_ERR, "Create new process for compile error" \
				" (%d).", errno);
				return 0;
			case 0:
				// Potlaceni vsech vystupu
				close_all_fds(-1);

				// Spusteni skriptu
				execlp("tl_compile", "tl_compile", release, \
				platforms[i][DB_PLATFORMS_ID], \
				platforms[i][DB_PLATFORMS_NAME], NULL);

				// Ukonceni programu v pripade chyby
				return 1;
			default:
				// Pocet bezicich potomku
				++childs;
				break;
		}
	}

	// Cekani na ukonceni vsech procesu tl_compile
	while(childs--){
		pid =  wait(&status);
	}

	/******************************************************************
	 *  Spusteni programu udrzujici TELNET, SSH spojeni s routerem
	 *****************************************************************/

	// Zmena pracovniho adresare
	result = chdir(DIRECTORY);
	if(result < 0){
		syslog(LOG_ERR, "Change directory to %s error (%d).", DIRECTORY, errno);
		return 1;
	}

	// Ziskani vsech routeru z databaze
	routers = database_sel_routers();
	if(routers == NULL){
		syslog(LOG_ERR, "Database error: select routers.");
		return 1;
	}

	// Inicializace poctu potomku
	childs_remote = 0;

	 // Prochazeni vsemi routry
	 for(i = 0; routers[i] != NULL; i++){

		 // Vytvoreni procesu pro komunikaci s routerem
		 switch(pid = vfork()){
			case -1:
				syslog(LOG_ERR, "Create new process for test error" \
				" (%d).", errno);
				return 1;
			 case 0:
				// Potlaceni vsech vystupu
				close_all_fds(-1);

				// Spusteni skriptu
				execlp("tl_remote_server", "tl_remote_server", \
				routers[i][DB_ROUTERS_ID], \
				routers[i][DB_ROUTERS_ADDRESS], \
				routers[i][DB_ROUTERS_PROTOCOL], NULL);

				// Ukonceni programu v pripade chyby
				return 1;
			default:
				// Pocet bezicich potomku
				++childs_remote;
				break;
		 }
	 }


	/******************************************************************
	 *  Testovani routeru
	 *****************************************************************/

	// Inicializace poctu potomku
	childs = 0;

	// Prochazeni vsemi routry
	for(i = 0; routers[i] != NULL; i++){

		// Vytvoreni procesu pro stazeni projektu
		switch (pid = vfork()) {
			case -1:
				syslog(LOG_ERR, "Create new process for test error" \
				" (%d).", errno);
				return 1;
			case 0:
				// Potlaceni vsech vystupu
				close_all_fds(-1);

				// Spusteni skriptu
				execlp("tl_test", "tl_test", release, \
				routers[i][DB_ROUTERS_ID], NULL);

				syslog(LOG_ERR, "Script test no running.");

				// Ukonceni programu v pripade chyby
				return 1;
			default:
				// Pocet bezicich potomku
				++childs;
				break;
		}
	}

	// Cekani na ukonceni vsech procesu tl_test
	while(childs--){
		pid =  wait(&status);
	}

	/******************************************************************
	 *  Testovani tunelu
	 *****************************************************************/



	/******************************************************************
	 *  Ukonceni testu - uklid projektu
	 *****************************************************************/

	// Inicializace poctu potomku
	childs = 0;

	 // Prochazeni vsemi routry
	 for(i = 0; routers[i] != NULL; i++){

		 // Ukonceni procesu pro komunikaci s routerem
		 switch(pid = vfork()){
			case -1:
				syslog(LOG_ERR, "Create new process for test error" \
				" (%d).", errno);
				return 1;
			 case 0:
				// Potlaceni vsech vystupu
				close_all_fds(-1);

				// Spusteni skriptu
				execlp("tl_remote_exit", "tl_remote_exit", \
				routers[i][DB_ROUTERS_ID], NULL);

				// Ukonceni programu v pripade chyby
				return 1;
			default:
				// Pocet bezicich potomku
				++childs;
				break;
		 }
	 }

	// Cekani na ukonceni vsech procesu tl_remote_exit
	while(childs--){
		pid = wait(&status);
	}

	// Cekani na ukonceni vsech procesu tl_remote
	while(childs_remote--){
		pid = wait(&status);
	}

	// Uvolneni pameti routers
	if(routers != NULL){
		database_results_free(routers);
		routers = NULL;
	}

	// Adresar pro logy distcleanu
	snprintf(directory, sizeof(directory), "%s/logs/clean/%lld", DIRECTORY, \
	release_id);

	// Vytvoreni adresare pro logy distcleanu
	mkdir(directory, S_IRWXU);
	if(result < 0){
		syslog(LOG_ERR, "Create logs directory error (%d)", errno);
		return 1;
	}

	// Inicializace poctu potomku
	childs = 0;

	// Prochazeni vsemi platformami
	for(i = 0; platforms[i] != NULL; i++){

		// Vytvoreni procesu pro distclean projektu
		switch (pid = vfork()) {
			case -1:
				syslog(LOG_ERR, "Create new process for clean error" \
				" (%d).", errno);
				return 0;
			case 0:
				// Potlaceni vsech vystupu
				close_all_fds(-1);

				// Spusteni skriptu
				execlp("tl_clear", "tl_clear", release, \
				platforms[i][DB_PLATFORMS_ID], \
				platforms[i][DB_PLATFORMS_NAME], NULL);

				// Ukonceni programu v pripade chyby
				return 1;
			default:
				// Pocet bezicich potomku
				++childs;

				break;
		}
	}

	// Cekani na ukonceni vsech procesu
	while(childs--){
		pid =  wait(&status);
	}

	// Uvolneni pameti
	if(platforms != NULL){
		database_results_free(platforms);
		platforms = NULL;
	}

	// Zmena pracovniho adresare
	result = chdir(DIRECTORY);
	if(result < 0){
		syslog(LOG_ERR, "Change directory to %s error (%d).", \
		DIRECTORY, errno);
		return 1;
	}

	// Smazani projektu
	project_delete(release_id);

	// Zavreni logu
	closelog();

	return 0;
}
