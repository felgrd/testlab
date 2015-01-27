#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "cspipe.h"
#include "database.h"

/// The default location of the firmware
#define	DEFAULT_IMAGEDIR "/var/testlab/firmware"
/// The default location of the configuration
#define DEFAULT_CONFDIR "/var/testlab/conf"
/// The default login username
#define DEFAULT_USER "root"
/// The default login passwordmake
#define	DEFAULT_PASS "root"

/**
 * @file tl_update.c
 *
 * @brief usage tl_updatefw -r \<release\> [-d \<dir\>] [-u \<user\>]
 * [-p \<pass\>] [-i \<ip\> -f \<firmware\> -t \<protocol\>] or [\<ID\>]
 * <br> tl_updateconf -r \<release\> [-d \<dir\>] [-u \<user\>]
 * [-p \<pass\>] [-i \<ip\> -f \<firmware\> -t \<protocol\>] or [\<ID\>]
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 5.1.2015
 *
 * Program tl_updatefw update new firmware to the router.
 * Program tl_updateconf update new configuration to the router.
 * New firmware or configuration is uploaded via http or https protocol.
 * Program is required id of router or ip address, name of firmware and
 * connection protocol.
 *
 * @param -i \<ip\> IP address of the router updated.
 * @param -f \<firmware\> Name of new firmware or configuration.
 * @param -d \<dir\> Directory of new firmware or configuration.
 * @param -u \<user\> User login your tested router.
 * @param -p \<pass\> Password login your tested router.
 * @param -r \<release\> Release id of your tested router.
 * @param -t \<protocol\> Protocol of connectivity your tested router.
 * @param \<id\> Router id of your tested router.
 *
 * @returns The return value is the same as curl. http://curl.haxx.se/docs/manpage.html
 *
 * @cond
 */

void help(void){
	#ifdef FW
		printf("usage tl_updatefw -r <release> -f <firmware> [-i <ip> " \
		" -t <protocol>] or  [<ID>] [-d <dir>] [-u <user>] [-p <pass>]\n");
	#elif CONF
		printf("usage tl_updateconf -r <test> -f <configuration> [-i <ip> " \
		" -t <protocol>] or [<ID>] [-d <dir>] [-u <user>] [-p <pass>]\n");
	#endif
}

int main(int argc, char *argv[]){
	char      ip[20];
	char      firmware[PATH_MAX];
	char      imagedir[PATH_MAX];
	char      user[20];
	char      pass[20];
	char      release[20];
	char      protocol[20];
	char      parameter;
	char      command[255];
	char      file[255];
	int       result;
	int       router;

	// Inicializace promenych
	#ifdef FW
		strncpy(imagedir, DEFAULT_IMAGEDIR, sizeof(imagedir));
	#elif CONF
		strncpy(imagedir, DEFAULT_CONFDIR, sizeof(imagedir));
	#endif

	strncpy(user, DEFAULT_USER, sizeof(user));
	strncpy(pass, DEFAULT_PASS, sizeof(pass));
	release[0] = '\0';
	firmware[0] = '\0';
	ip[0] = '\0';
	protocol[0] = '\0';

	// Rozbor parametru na prikazove radce
	while ((parameter = getopt(argc, argv, "i:f:d:u:p:r:t:")) != -1) {
		switch (parameter) {
			case 'i':
				strncpy(ip, optarg, sizeof(ip));
				break;
			case 'f':
				strncpy(firmware, optarg, sizeof(firmware));
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
	if(!strlen(ip) || !strlen(protocol)){

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
	if(!strlen(ip)){

		result = pipe_request(remote_status_address, NULL, ip);
		if(!result){
			fprintf(stderr, "No response from router.\n");
			return 1;
		}
	}

	// Ziskani komunikacniho protokolu pokud nebyl zadan
	if(!strlen(protocol)){

		result = pipe_request(remote_status_protocol, NULL, protocol);
		if(!result){
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
	if(strcmp(protocol, "ssh") == 0){
		execlp("curl", "curl", "--ssl", "-k", file, command, NULL);
	}else{
		execlp("curl", "curl", file, command, NULL);
	}

	return 1;
}
