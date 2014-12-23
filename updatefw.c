#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "cspipe.h"
#include "database.h"

#define	DEFAULT_IMAGEDIR	"/var/testlab/firmware"
#define DEFAULT_CONFDIR		"/var/testlab/conf"
#define DEFAULT_USER		"root"
#define	DEFAULT_PASS		"root"

void help(void){
	printf("usage tl_updatefw\n");
}

/**
 * ## @package confupdate
#  This module upload configuration to router.
#  
#  @param -r <router> Address of router.
#  @param -f <firmware> Firmware of router.
#  @param [-c <confdir>] The directory where the desired configuration.
# The default directory is ./configuration.
#  @param [-u <user>] User name of router. The default user is root.
#  @param [-p <pass>] Password of router. The default password is root.
#  @param -h Print help.

 * */

int main(int argc, char *argv[]){
	int				router;
	char			ip[20];
	char			firmware[PATH_MAX];
	char			imagedir[PATH_MAX];
	char			user[20];
	char			pass[20];
	char			release[20];
	char			protocol[20];	
	char			parameter;	
	int				selIP;
	int				selFW;	
	int				selPR;
	char			*product;
	int 			result;
	int				fifo_fd;	
	char			command[255];
	char			file[255];	
	message_remote	remote_request;		// Format posilanych dat
	message_remote	remote_response;	// Format posilanych dat
	
	// Inicializace promenych
	selIP = 0;
	selFW = 0;	
	selPR = 0;
	
	#ifdef FW	
		strncpy(imagedir, DEFAULT_IMAGEDIR, sizeof(imagedir));
	#elif CONF
		strncpy(imagedir, DEFAULT_CONFDIR, sizeof(imagedir));
	#endif	
	
	strncpy(user, DEFAULT_USER, sizeof(user));
	strncpy(pass, DEFAULT_PASS, sizeof(pass));
	release[0] = '\0';
		
	// Rozbor parametru na prikazove radce
	while ((parameter = getopt(argc, argv, "i:f:d:u:p:r:t:")) != -1) {
		switch (parameter) {
			case 'i':
				strncpy(ip, optarg, sizeof(ip));
				selIP++;
				break;
			case 'f':
				strncpy(firmware, optarg, sizeof(firmware));
				selFW++;
				break;
			case 'd':
				strncpy(imagedir, optarg, sizeof(imagedir));
				break;
			case 'u':
				strncpy(user, optarg, sizeof(user));
				break;
			case 'p':
				strncpy(pass, optarg, sizeof(pass));
				break;
			case 'r':
				strncpy(release, optarg, sizeof(release));
				break;
			case 't':
				strncpy(protocol, optarg, sizeof(protocol));
				selPR++;
				break;
			case '\?':
				help();
				return 0;
			case ':':
				help();
				return 0;
		}
	}
	
	// Pokud nebyla specifikovana IP adresa nebo FW
	if(!selIP || !selFW || selPR){
	
		// Kontrola parametru id routeru
		if(optind >= argc){
			help();
			return 0;
		}
	
		// Kontola platneho hodnoty parametru
		router = atoi(argv[optind]);
		if(router <= 0){
			help();
			return 0;
		}	
	}
	
	// Ziskani IP adresy pokud nebyla zadana
	if(!selIP){
		
		// Nastaveni pozadavku
		remote_request.request = remote_status_address;
		
		// Navazani spojeni
		fifo_fd = client_starting(router);
		if(fifo_fd){
			// Odelani zadosti
			result = send_mess_to_server(fifo_fd, remote_request);

			// Prijem odpovedi
			if(result){
				read_response_from_server(&remote_response);			
			}
		}

		// Ukonceni komunikace
		client_ending(router, fifo_fd);	

		// Ziskani vysledku
		if(remote_response.request == remote_response_ok){
			strncpy(ip, remote_response.data, sizeof(ip));
		}else{
			fprintf(stderr, "No response from router.\n");
			return 1;
		}		
	}

	// Ziskani nazvu fw pokud nebyl zadan
	if(!selFW){
		
		// Provedeni SQL dotazu
		product = database_sel_product(router);
	
		// Kontrola vysledku dotazu
		if(product == NULL){
			return 1;
		}
		
		strncpy(firmware, product, sizeof(firmware));
		
		free(product);
	}
	
	// Ziskani komunikacniho protokolu pokud nebyl zadan
	if(!selPR){
		
		// Nastaveni pozadavku
		remote_request.request = remote_status_protocol;
		
		// Navazani spojeni
		fifo_fd = client_starting(router);
		if(fifo_fd){
			// Odelani zadosti
			result = send_mess_to_server(fifo_fd, remote_request);

			// Prijem odpovedi
			if(result){
				read_response_from_server(&remote_response);			
			}
		}

		// Ukonceni komunikace
		client_ending(router, fifo_fd);	

		// Ziskani vysledku
		if(remote_response.request == remote_response_ok){
			
			if(strcmp(remote_response.data, "ssh") == 0){
				strcpy(protocol, "s");
			}else{
				strcpy(protocol, "");
			}
		}else{
			fprintf(stderr, "No response from router.\n");
			return 1;
		}		
	}
	
	#ifdef FW	
		// Sestaveni umisteni nahravaneho souboru
		snprintf(file, sizeof(file), "-F file=@%s/%s/%s.bin", imagedir, \
		release, firmware);	
		
		// Sestaveni adresy
		snprintf(command, sizeof(command), \
		"http%s://%s:%s@%s/update_exec.cgi", protocol, user, pass, ip);
		
	#elif CONF	
		// Sestaveni umisteni nahravaneho souboru
		snprintf(file, sizeof(file), "-F file=@%s/%s.cfg", imagedir, \
		firmware);
		
		// Sestaveni adresy
		snprintf(command, sizeof(command), \
		"http%s://%s:%s@%s/restore_exec.cgi", protocol, user, pass, ip);
		
	#endif
	
	// Potlaceni vsech vystupu
	close_all_fds(-1);
	
	// Spusteni update fw nebo conf
	if(strcmp(protocol, "s") == 0){
		execlp("curl", "curl", "--ssl", "-k", file, command, NULL);
	}else{
		execlp("curl", "curl", file, command, NULL);
	}
	
	
		
	return 1;
}
