#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // getopt
#include "database.h"

/**
 * @file tl_interface.c
 *
 * @brief usage tl_interface [-i] [-n] [-p] [-t \<type\>] \<id\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 3.2.2015
 *
 * Program returns selected information about interface in tested routers. <br>
 * Example command: tl_interface -i 4. Answer: 172.16.5.25.
 *
 * @param -i Program returns IP address of interface.
 * @param -n Program return netmask of interface.
 * @param -p Program return port of switch.
 * @param -t\<type\> Parameter type selected which interface in router is
 * selected. Default interface is Primary Lan (1).
 * @param \<id\> Router ID of tested router.
 *
 * @return 0 - Return value is valid<br>
 *         1 - Parameter is not valid<br>
 *         2 - Return value is not valid
 *
 * @cond
 */

void help(void){
  fprintf(stderr, "usage tl_interface [-i] [-n] [-p] [-t <type>] <id>\n");
}

int main(int argc, char *argv[]){
  int     router;             // Identifikator routeru
  int     parameter;          // Zpracovani prepinacu programu
  int     command;            // Příkaz provedený routerem
  int     position;           // Typ pozadovaneho interfacu
  char    **interface;        // Informace o interfacu

  // Inicializace stavu promenych
  command = DB_INTERFACES_ID;
  position = 1;

  // Rozbor parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "inpt:")) != -1){
    switch (parameter){
      case 'i':
        command = DB_INTERFACES_IP;
        break;
      case 'n':
        command = DB_INTERFACES_NETMASK;
        break;
      case 'p':
        command = DB_INTERFACES_PORT;
        break;
      case 't':
        position = atoi(optarg);
        if(position <= 0){
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

  // Ziskani data z databaze
  interface = database_sel_interface(router, position);

  // Kontrola dat z databaze
  if(interface == NULL || interface[command] == NULL){
    return 2;
  }

  // Tisk vysledku
  printf("%s", interface[command]);

  // Uvolneni pameti
  database_result_free(interface);

  return 0;
}
