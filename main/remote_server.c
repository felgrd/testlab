#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>      // syslog
#include <fcntl.h>       // open
#include <sys/types.h>   // pipe
#include <sys/stat.h>    // pipe
#include <sys/select.h>  // select
#include <errno.h>
#include "telnet.h"
#include "ssh.h"
#include "cspipe.h"

#define	PROTOCOL_TELNET   0
#define PROTOCOL_SSH      1

#define REMOTE_USER_LENGTH    20   // Velikost bufferu pro uzivatele
#define REMOTE_PASS_LENGTH    20   // Velikost bufferu pro heslo
#define REMOTE_IP_LENGTH      20   // Velikost bufferu pro IP adresu
#define REMOTE_PROT_LENGTH    20   // Velikost bufferu pro protokol

// Prijate cislo signalu
volatile int got_signal = 0;

// Obsluha prijmu signalu
static void sig_handler(int signum){
	got_signal = signum;
}

// Informace o spatne zadanych parametrech
void help(void){
	syslog(LOG_ERR, "Usage: remote [-u <user>] [-p <pass>] [-t <port>]" \
	"<id_router> <ip address> <protocol>.");
}

int main(int argc, char **argv ){
	int            telnet_fd;                // Deskriptor otevreneho telnet spojeni
	pid_t          ssh_fd;                   // Deskriptor otevreneho ssh spojeni
	int            result;                   // Navratovy kod funkce
	char           parameter;                // Predany parametr
	int            port;                     // Port na kterem je spojeni otevreno
	char           user[REMOTE_USER_LENGTH]; // Retezec pro zadani uzivatele
	char           pass[REMOTE_PASS_LENGTH]; // Reteze pro zadani hesla
	char           ip[REMOTE_IP_LENGTH];     // Retezec pro zadani ip adresy
	char           ip_arg[REMOTE_IP_LENGTH]; // Vychozi IP adresa
	int            router;                   // Identifikator routeru
	int            protocol;                 // Retezec pro zadani protokolu
	int            server_fifo_fd;           // Roura pro prijem dat
	int            client_fifo_fd;           // Roura pro odesilani dat
	int            fifo_read;                // Velikost prijatych dat
	int            fifo_write;               // Velikost odeslanych dat
	message_remote remote_request;           // Format posilanych dat
	message_remote remote_response;          // Format posilanych dat
	char           server_pipe_name[PATH_MAX + 1];
	char           client_pipe_name[PATH_MAX + 1];
	char           result_buffer[512];         // Navratovy kod funkce na zarizeni

	// Inicializace parametru
	server_fifo_fd = -1;
	telnet_fd = 0;
	ssh_fd = 0;
	result = 0;
	port = TELNET_PORT;
	protocol = PROTOCOL_TELNET;
	strncpy(user, TELNET_USER, sizeof(user));
	strncpy(pass, TELNET_PASS, sizeof(pass));

	// Instalace signalu
	signal(SIGTERM, sig_handler);

	// Otevreni syslogu
	openlog ("TL_REMOTE_SERVER", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	// Rozbor parametru na prikazove radce
	while ((parameter = getopt(argc, argv, "u:p:t:")) != -1) {
		switch (parameter) {
			case 'u':
				strncpy(user, optarg, sizeof(user));
				break;
			case 'p':
				strncpy(pass, optarg, sizeof(pass));
				break;
			case 't':
				port = atoi(optarg);
				if(port <= 0){
					help();
					return 1;
				}
				break;
			case '\?':
				help();
				return 1;
			case ':':
				help();
				return 1;
		}
	}

	// Kontrola parametru ip address
	if(optind + 2 >= argc){
		help();
		return 1;
	}

	// Parametr identifikacni cislo pro komunikace se serverem
	router = atoi(argv[optind++]);
	if(router <= 0){
		syslog(LOG_ERR, "Parameter id is not number.");
		return 1;
	}

	// Parametr IP adresa routeru
	strncpy(ip, argv[optind], sizeof(ip));
	strncpy(ip_arg, argv[optind], sizeof(ip_arg));
	++optind;

	// Parametr protokol pro komunikaci s routerem
	protocol = strcmp(argv[optind], "ssh") == 0 ? PROTOCOL_SSH : PROTOCOL_TELNET;

	// Vytvoreni komunikacnich rour
	snprintf(server_pipe_name, sizeof(server_pipe_name), SERVER_FIFO_NAME, router);
	result = mkfifo(server_pipe_name, 0777);
	if(result != 0){
		syslog(LOG_ERR, "Create fifo error (%d)", errno);
		return 1;
	}

	// Cekani na prichozi pozadavek
	while(!got_signal){

		// Otereni serverove roury
		if(server_fifo_fd == -1){
			server_fifo_fd = open(server_pipe_name, O_RDONLY);
			if(server_fifo_fd == -1){
				syslog(LOG_ERR, "Server pipe open error %d %s.", errno, server_pipe_name);
				return 1;
			}
		}

		// Prijem prikazu
		fifo_read = read(server_fifo_fd, &remote_request, sizeof(remote_request));

		// Uzavreni roury ze strany klienta
		if(fifo_read == 0){
			close(server_fifo_fd);
			server_fifo_fd = -1;
			continue;
		}

		// Kontrola velikosti prijatych dat
		if(fifo_read != sizeof(remote_request)){
			syslog(LOG_ERR, "Server pipe read error. Read: %dB. " \
			"Expect: %luB.", fifo_read, sizeof(remote_request));
			continue;
		}

		// Zpracovani prikazu
		switch(remote_request.request){

			case remote_process:

				if(protocol == PROTOCOL_TELNET){

					// Sestaveni spojeni
					if(!telnet_fd){
						telnet_fd = telnetInit(ip, port, user, pass);
					}

					// Provedeni prikazu
					result = telnetExec(telnet_fd, remote_request.data, \
					remote_response.data, sizeof(remote_response.data));

					// Ziskani navratove hodnoty provedene funkce
					if(result){
						result = telnetExec(telnet_fd, "echo $?", result_buffer, \
						sizeof(result_buffer));

						remote_response.result_code = atoi(result_buffer);
					}

					// Uzavreni spojeni v pripade chyby
					if(!result && telnet_fd){
						telnetDone(telnet_fd);
						telnet_fd = 0;
					}
				}else if(protocol == PROTOCOL_SSH){

					// Sestaveni spojeni
					if(!sshCheck(ssh_fd)){
						ssh_fd = sshInit(ip, port, user, pass);
					}

					// Provedeni prikazu
					result = sshExec(ssh_fd, remote_request.data, \
					remote_response.data, sizeof(remote_response.data));

					// Ziskani navratove hodnoty provedene funkce
					if(result){
						result = sshExec(ssh_fd, "echo $?", result_buffer, \
						sizeof(result_buffer));

						remote_response.result_code = atoi(result_buffer);
					}

					// Uzavreni spojeni v pripade chyby
					if(!result && ssh_fd){
						sshDone(ssh_fd);
						ssh_fd = 0;
					}
				}

				// Nastaveni odpovedi
				remote_response.request = result ? remote_response_ok : \
				remote_response_error;
				break;

			case remote_change_address:
				// Nastaveni nove ip adresy
				strncpy(ip, remote_request.data, sizeof(ip));

				// Nastaveni odpovedi
				remote_response.request = port ? remote_response_ok : \
				remote_response_error;
				break;

			case remote_change_port:
				// Prevod portu na cislo
				port = atoi(remote_request.data);

				// Nastaveni odpovedi
				remote_response.request = port ? remote_response_ok : \
				remote_response_error;
				break;

			case remote_change_user:
				// Nastaveni noveho uzivatele
				strncpy(user, remote_request.data, sizeof(user));

				// Nastaveni odpovedi
				remote_response.request = remote_response_ok;
				break;

			case remote_change_pass:
				// Nastaveni noveho hesla
				strncpy(pass, remote_request.data, sizeof(pass));

				// Nastaveni odpovedi
				remote_response.request = remote_response_ok;
				break;

			case remote_change_protocol:
				// Nastavemi noveho komunikacniho protokolu
				if(strcmp(remote_request.data, "telnet") == 0){
					protocol = PROTOCOL_TELNET;
					remote_response.request = remote_response_ok;
				}else if(strcmp(remote_request.data, "ssh") == 0){
					protocol = PROTOCOL_SSH;
					remote_response.request = remote_response_ok;
				}else{
					remote_response.request = remote_response_error;
				}

				break;

			case remote_default_conf:
				// Nastaveni defaultni konfigurace
				strncpy(ip, ip_arg, sizeof(ip));
				port = TELNET_PORT;
				strncpy(user, TELNET_USER, sizeof(user));
				strncpy(pass, TELNET_PASS, sizeof(pass));

				// Nastaveni odpovedi
				remote_response.request = remote_response_ok;
				break;

			case remote_status_address:
				// Odeslani nastavene ip adresy
				strncpy(remote_response.data, ip, \
				sizeof(remote_response.data));
				remote_response.request = remote_response_ok;
				break;

			case remote_status_port:
				// Odeslani nastaveneho portu
				snprintf(remote_response.data, \
				sizeof(remote_response.data), "%d", port);
				remote_response.request = remote_response_ok;
				break;

			case remote_status_user:
				// Odeslani nastaveneho uzivatele
				strncpy(remote_response.data, user, \
				sizeof(remote_response.data));
				remote_response.request = remote_response_ok;
				break;

			case remote_status_pass:
				// Odeslani nastaveneho hesla
				strncpy(remote_response.data, pass, \
				sizeof(remote_response.data));
				remote_response.request = remote_response_ok;
				break;

			case remote_status_protocol:
				// Odeslani nastaveneho protokolu
				if(protocol == PROTOCOL_TELNET){
					strcpy(remote_response.data, "telnet");
				}else if(protocol == PROTOCOL_SSH){
					strcpy(remote_response.data, "ssh");
				}
				remote_response.request = remote_response_ok;
				break;

			case remote_reconnect:
				// Uzavreni telnet spojeni
				if(protocol == PROTOCOL_TELNET && telnet_fd){
					telnetDone(telnet_fd);
					telnet_fd = 0;
				}else if(protocol == PROTOCOL_SSH && sshCheck(ssh_fd)){
					sshDone(ssh_fd);
					ssh_fd = 0;
				}

				// Nastaveni odpovedi
				remote_response.request = remote_response_ok;
				break;

			case remote_close_conn:
				// Nastaveni signalu ukonceni
				got_signal = SIGTERM;

				// Nastaveni odpovedi
				remote_response.request = remote_response_ok;
				break;

			default:
				// Nastaveni chybove odpovedi odpovedi
				remote_response.request = remote_response_error;
				break;
		}

		// Otevreni klientske roury
		snprintf(client_pipe_name, sizeof(client_pipe_name), \
		CLIENT_FIFO_NAME, remote_request.client_pid);
		client_fifo_fd = open(client_pipe_name, O_WRONLY);
		if(client_fifo_fd == -1){
			syslog(LOG_ERR, "Client pipe create error.");
			return 1;
		}

		// Odeslani odpovedi
		fifo_write = write(client_fifo_fd, &remote_response, \
		sizeof(remote_response));
		if(fifo_write != sizeof(remote_response)){
			syslog(LOG_ERR, "Client pipe send error.");
			return 1;
		}

		// Uzavreni klientske roury
		close(client_fifo_fd);
		client_fifo_fd = -1;
	}

	// Uzavreni telnet spojeni
	if(telnet_fd){
		telnetDone(telnet_fd);
		telnet_fd = 0;
	}

	// Uzavreni ssh spojeni
	if(sshCheck(ssh_fd)){
		sshDone(ssh_fd);
		ssh_fd = 0;
	}

	// Uzavreni komunikacnich rour
	unlink(server_pipe_name);

	// Uzavreni logu
	closelog();

	return 0;
}
