#include <unistd.h>      // pipe
#include <stdio.h>       // snprintf
#include <string.h>      // strcat
#include <signal.h>      // kill
#include <sys/select.h>  // select
#include <sys/types.h>   // select
#include <sys/wait.h>    // waitpid
#include "ssh.h"


int sshInit(char *ip, int port, const char *username, const char *password){
	int             result;         // Vysledek provadene funkce
	pid_t           pid;            // PID procesu PLINK
	fd_set          read_fd_set;    // Mnozina deskriptoru pro SELECT
	ssize_t         length;         // Velikost prectenych dat
	char            command[255];   // Prikaz spusteni programu plink
	struct timeval  tv;             // Timeout pro navazani spojeni

	// Vytvoreni roury pro predavani hodnot
	if(pipe(pipes_stdin) == -1){
		return 0;
	}

	// Vytvoreni roury pro predavani hodnot
	if(pipe(pipes_stdout) == -1){
		return 0;
	}

	// Sestaveni commandu
	snprintf(command, sizeof(command), "%s@%s", username, ip);

	// Vytvoreni procesu starajici se o SSH spojeni
	pid = vfork();

	switch(pid){
		case -1:
			return 0;
			break;

		case 0:
			// Nastaveni vstupu a vystup programu PLINK
			close(0);
			dup(pipes_stdin[0]);
			close(1);
			dup(pipes_stdout[1]);
			close(2);
			close(pipes_stdin[0]);
			close(pipes_stdin[1]);
			close(pipes_stdout[0]);
			close(pipes_stdout[1]);

			// Spusteni programu plink
			execlp("plink", "plink", "-pw", password, command, NULL);

			break;

		default:
			// Uzavreni nepouzivanych rour
			close(pipes_stdin[0]);
			close(pipes_stdout[1]);

			// Nastaveni timeoutu ssh pripojeni
			tv.tv_sec = SSH_CONNECT_TIMEOUT;
			tv.tv_usec = 0;

			FD_ZERO(&read_fd_set);
			FD_SET(pipes_stdout[0], &read_fd_set);

			// Cekani na prijem dat ze standatrniho vystupu PLINK
			result = select(pipes_stdout[0] + 1, &read_fd_set, NULL, \
			NULL, &tv);

			if(result){
				// Precteni prijatych dat
				length = read(pipes_stdout[0], command, sizeof(command));
				command[length] = '\0';

				// Kontrola korektne navazaneho spojeni
				if(strstr(command, "#") != NULL){
					return pid;
				}
			}

			break;
	}

	// Uzavreni PLINK deamona v pripade neuspesneho pripojeni
	sshDone(pid);

	return 0;
}

int sshCheck(pid_t pid){
	int   result;   // Navratova hodnato funkce
	int   status;   // Status ukonceneho procesu
	pid_t wpid;     // PID ukonceneho procesu

	// Kontrola platnosti pid
	if(pid <= 0){
		return 0;
	}

	// Kontrola behu programu plink
	result = kill(pid, 0);
	if(result < 0){
		return 0;
	}

	// Ukonceni zoombie
	wpid = waitpid(pid, &status, WNOHANG);

	return wpid == 0;
}

int sshDone(pid_t pid){
	int   result;   // Navratova hodnato funkce
	int   status;   // Status ukonceneho procesu
	pid_t wpid;     // PID ukonceneho procesu

	// Ukonceni PLINK
	result = kill(pid, SIGTERM);
	if(result < 0){
		return 0;
	}

	// Cekani na ukonceni procesu
	wpid = waitpid(pid, &status, 0);

	return wpid == pid;
}

int sshExec(pid_t pid, char *request, char *response, int length){
	int             rcvLength;
	int             rspLength;
	char            *start;
	char            *stop;
	char            rcvBuffer[SSH_RCV_BUFFER];
	char            strBuffer[SSH_STR_BUFFER];
	fd_set          read_fd_set;
	int             result;
	struct timeval  tv;

	// Kontrola sestaveneho spojeni
	if(!sshCheck(pid)){
		strcpy(response, "SSH error: Program PLINK is not running.\n");
		return 0;
	}

	// Vytvoreni retezce z bufferu
	strBuffer[0] = '\0';

	// Pripojeni konce radku k zadosti
	strcat(request, "\r\n");

	// Odeslani zadosti programu PLINK
	rcvLength = write(pipes_stdin[1], request, strlen(request));

	// Nastaveni timeoutu prijmu dat
	tv.tv_sec = SSH_RECEIVE_TIMEOUT;
	tv.tv_usec = 0;

	do{
		FD_ZERO(&read_fd_set);
		FD_SET(pipes_stdout[0], &read_fd_set);

		// Cekani na prijem dat ze standatrniho vystupu PLINK
		result = select(pipes_stdout[0] + 1, &read_fd_set, NULL, NULL, &tv);

		// Prijem dat ze standartniho vystupu scriptu
		if(result){

			// Cteni dat
			if(FD_ISSET(pipes_stdout[0], &read_fd_set)){

				rcvLength = read(pipes_stdout[0], rcvBuffer, sizeof(rcvBuffer) - 1);

				// Kontrola platnosti dat
				if(rcvLength < 0){
					strcpy(response, "SSH error: Error while read data.\n");
					return 0;
				}
			}

			// Kontrola velikosti prijmaciho bufferu
			if(strlen(strBuffer) + rcvLength > sizeof(strBuffer)){
				strcpy(response, "SSH error: Output si too large.\n");
				return 0;
			}

			// Pripojeni prijatych data do prijmaciho bufferu
			rcvBuffer[rcvLength] = '\0';
			strncat(strBuffer, rcvBuffer, sizeof(strBuffer));

		// Data nebyla prijata do vyprseni timeoutu
		}else{
			strcpy(response, "SSH error: Read timeout expired.\n");
			return 0;
		}

	}while(strstr(strBuffer, "\r\n# ") == NULL);

	// Zpracovani odpovedi
	start = strstr(strBuffer, request);
	stop = strstr(strBuffer, "\r\n# ");

	// Kontrola validni odpovedi
	if(start == NULL || stop == NULL) {
		strcpy(response, "SSH error: Answer is not valid.\n");
		return 0;
	}

	start += strlen(request);
	stop += 2;

	// Kontrola existence odpovedi
	if(start == stop) {
		strcpy(response, "SSH error: No answer.\n");
		return 0;
	}

	// Kontrola validni odpovedi
	if(start > stop) {
		strcpy(response, "SSH error: Answer is not valid.\n");
		return 0;
	}

	// Kontrola velikosti odpovedi
	rspLength = stop - start;
	if(rspLength > length){
		rspLength = length;
	}

	// Kopirovani odpovedi
	strncpy(response, start, rspLength);
	response[rspLength] = '\0';

	return 1;
}
