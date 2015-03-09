#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // getopt
#include "database.h"

/**
 * @file tl_sim.c
 *
 * @brief usage tl_sim [-i] [-n] [-o] [-c] [-k] [-p \<position\>] \<id\>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 3.2.2015
 *
 * Program returns selected information about SIM cards in tested routers. <br>
 * Example command: tl_sim -i 4. Answer: 739541339.
 *
 * @param -i Program returns IP address of sim card.
 * @param -n Program return phone number of SIM card.
 * @param -o Program return operator of SIM card.
 * @param -c Program return PIN of SIM card.
 * @param -k Program return PUK of SIM card.
 * @param -p\<postion\> Parameter position selected which SIM card in router is
 * selected. Default position is 1.
 * @param \<id\> Router ID of tested router.
 *
 * @return 0 - Return value is valid<br>
 *         1 - Parameter is not valid<br>
 *         2 - Return value is not valid
 *
 * @cond
 */

void help(void){
  fprintf(stderr, "usage tl_sim [-i] [-n] [-o] [-c] [-k] [-p <position>] <id>\n");
}

int main(int argc, char *argv[]){
  int     router;             // Identifikator routeru
  int     parameter;          // Zpracovani prepinacu programu
  int     command;            // Příkaz provedený routerem
  int     position;           // Pozice vybrane SIM karty
  char    **sim;              // Informace o SIM karte

  // Inicializace stavu promenych
  command = DB_SIMS_ID;
  position = 1;

  // Rozbor parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "inockp:")) != -1){
    switch (parameter){
      case 'i':
        command = DB_SIMS_IP;
        break;
      case 'n':
        command = DB_SIMS_NUMBER;
        break;
      case 'o':
        command = DB_SIMS_OPERATOR;
        break;
      case 'c':
        command = DB_SIMS_PIN;
        break;
      case 'k':
        command = DB_SIMS_PUK;
        break;
      case 'p':
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
  sim = database_sel_sim(router, position);

  // Kontrola dat z databaze
  if(sim == NULL || sim[command] == NULL){
    return 2;
  }

  // Tisk vysledku
  printf("%s", sim[command]);

  // Uvolneni pameti
  database_result_free(sim);

  return 0;
}
