#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "utils.h"
#include "cspipe.h"

/// The default location of the firmware
#define	DEFAULT_IMAGEDIR "/var/testlab/firmware"

/**
 * @file tl_updatefw.c
 *
 * @brief usage tl_updatefw -r \<release\> -f \<firmware\> [-d \<dir\>] \<id\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 29.1.2015
 *
 * Program tl_updateconf update new firmware to the router. <br>
 * Example command: tl_updateconf -r 4 -f LR77-v2 1. Router update firmware
 * /var/testlab/firmware/4/LR77-v2.bin to the router with ID 1.
 *
 * @param -r \<release\> Release of tested firmware.
 * @param -f \<firmware\> Name of firmware.
 * @param -d \<dir\> Directory of new firmware.
 * @param \<id\> Router id of your tested router.
 *
 * @returns The return value is the same as curl.
 * http://curl.haxx.se/docs/manpage.html
 *
 * @cond
 */

void help(void){
  printf("usage tl_updatefw -r <release> -f <firmware> [-d <dir>] <router>\n");
}


int main(int argc, char *argv[]){
  char    imagedir[PATH_MAX]; // Cesta k adresari skriptu
  char    firmware[PATH_MAX]; // Nazev nahravaneho firmware
  int     release;            // Identifikator testovaneho releasu
  int     router;             // Identifikator testovaneho routeru
  char    user[30];           // Prihlasovaci jmeno k routeru
  char    pass[30];           // Prihlasovaci heslo k routeru
  char    ip[50];             // IP adresa routeru
  char    protocol[30];       // Protokol pristupu do routeru
  char    file[250];
  char    command[250];
  int     parameter;
  int     result;

  // Inicializace promenych
  strncpy(imagedir, DEFAULT_IMAGEDIR, sizeof(imagedir));
  firmware[0] = '\0';
  release = 0;

  // Rozbor parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "r:f:d:")) != -1) {
    switch (parameter) {
      case 'r':
        release = atoi(optarg);
        break;
      case 'f':
        strncpy(firmware, optarg, sizeof(firmware));
        break;
      case 'd':
        strncpy(imagedir, optarg, sizeof(imagedir));
        break;
      case '\?':
        help();
        return 1;
      case ':':
        help();
        return 1;
    }
  }

  // Kontrola parametru release
  if(release <= 0){
    help();
    return 1;
  }

  // Kontrola parametru firmware
  if(strlen(firmware) == 0){
    help();
    return 1;
  }

  // Kontrola parametru id routeru
  if(optind >= argc){
    help();
    return 1;
  }

  // Kontola platneho hodnoty parametru
  router = atoi(argv[optind]);
  if(router <= 0){
    help();
    return 1;
  }
  // Ziskani IP adresy
  result = pipe_request(router, remote_status_address, NULL, ip);
  if(!result){
    fprintf(stderr, "No response from router.\n");
    return 1;
  }

  // Ziskani komunikacniho protokolu
  result = pipe_request(router, remote_status_protocol, NULL, protocol);
  if(!result){
    fprintf(stderr, "No response from router.\n");
    return 1;
  }

  // Uprava komunikacniho protokolu
  if(strcmp(protocol, "ssh") == 0){
    strcpy(protocol, "s");
  }else{
    protocol[0] = '\0';
  }

  // Ziskani prihlasovaciho jmena
  result = pipe_request(router, remote_status_user, NULL, user);
  if(!result){
    fprintf(stderr, "No response from router.\n");
    return 1;
  }

  // Ziskani prihlasovaciho hesla
  result = pipe_request(router, remote_status_pass, NULL, pass);
  if(!result){
    fprintf(stderr, "No response from router.\n");
    return 1;
  }

  // Sestaveni umisteni nahravaneho souboru
  snprintf(file, sizeof(file), "-F file=@%s/%d/%s.bin", imagedir, \
  release, firmware);

  // Sestaveni adresy
  snprintf(command, sizeof(command), \
  "http%s://%s:%s@%s/update_exec.cgi", protocol, user, pass, ip);

  // Potlaceni vsech vystupu
  close_all_fds(-1);

  // Spusteni update fw nebo conf
  if(strcmp(protocol, "s") == 0){
    execlp("curl", "curl", "--ssl", "-k", file, command, NULL);
  }else{
    execlp("curl", "curl", file, command, NULL);
  }

  // V pripade nespusteni programu curl
  fprintf(stderr, "Program curl is not running.\n");

  return 1;
}
