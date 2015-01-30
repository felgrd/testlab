#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "utils.h"
#include "cspipe.h"

/// The default location of the configuration
#define DEFAULT_CONFDIR "/var/testlab/conf"

/**
 * @file tl_updateconf.c
 *
 * @brief usage tl_updateconf -f \<config\> -t \<function\> [-d \<confdir\>]
 * \<id\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 29.1.2015
 *
 * Program tl_updateconf update new configuration to the router. <br>
 * Example command: tl_updateconf -f test1 -f dyndns 1. Router upload
 * configuration in /var/testlab/conf/dyndns/1/test1.cfg to the router.
 *
 * @param -f \<config\> Name of configuration.
 * @param -t \<function\> Name of tested function.
 * @param -d \<dir\> Directory of new configuration.
 * @param \<id\> Router id of your tested router.
 *
 * @returns The return value is the same as curl.
 * http://curl.haxx.se/docs/manpage.html
 *
 * @cond
 */

void help(void){
  printf("usage tl_updateconf -f <config> -t <function> [-d <confdir>] <router>\n");
}

int main(int argc, char *argv[]){
  char    confdir[PATH_MAX];  // Cesta k adresari skriptu
  char    config[PATH_MAX];   // Konfigurace nahravana do routeru
  char    function[PATH_MAX]; // Testovana funkce
  char    user[30];           // Prihlasovaci jmeno k routeru
  char    pass[30];           // Prihlasovaci heslo k routeru
  char    ip[50];             // IP adresa routeru
  char    protocol[30];       // Protokol pristupu do routeru
  int     router;             // Identifikator routeru
  char    file[250];
  char    command[250];
  int     parameter;
  int     result;

  // Inicializace promenych
  strncpy(confdir, DEFAULT_CONFDIR, sizeof(confdir));
  config[0] = '\0';
  function[0] = '\0';

  // Rozbor parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "f:d:t:")) != -1) {
    switch (parameter) {
      case 'f':
        strncpy(config, optarg, sizeof(config));
        break;
      case 'd':
        strncpy(confdir, optarg, sizeof(confdir));
        break;
      case 't':
        strncpy(function, optarg, sizeof(function));
        break;
      case '\?':
        help();
        return 1;
      case ':':
        help();
        return 1;
    }
  }

  // Kontrola parametru function
  if(strlen(function) == 0){
    help();
    return 1;
  }

  // Kontrola parametru config
  if(strlen(config) == 0){
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
  snprintf(file, sizeof(file), "-F file=@%s/%s/%d/%s.cfg", confdir, function, \
  router, config);

  // Sestaveni adresy
  snprintf(command, sizeof(command), "http%s://%s:%s@%s/restore_exec.cgi", \
  protocol, user, pass, ip);

  // Potlaceni vsech vystupu
  close_all_fds(-1);

  // Spusteni update fw nebo conf
  if(strcmp(protocol, "s") == 0){
    execlp("curl", "curl", "--ssl", "-k", file, command, NULL);
  }else{
    execlp("curl", "curl", file, command, NULL);
  }

  // V pripade nespusteni programu curl
  fpritnf(stderr, "Program curl is not running.\n");

  return 1;
}
