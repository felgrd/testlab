#include <stdio.h>
#include <string.h>
#include <stdlib.h>     // atoi
#include <unistd.h>     // getopt
#include "cspipe.h"

/**
 * @file tl_pingb.c
 *
 * @brief usage tl_pingb -c <count> [-s <size>] [-i <iface>] <id> <address>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 4.3.2015
 *
 * Program tl_pingb starts ping to . <br>
 * Example command: tl_pingb -c 10 8 10.0.0.1.
 * Answer: Start ping from router 8 to address 10.0.0.1.
 *
 * @param <id> ID of router.
 *
 * @return 0 - Answer is valid.<br>
 *         1 - Parameter is not valid.<br>
 *         2 - Timeout is expired.
 *
 * @cond
 */

void help(void){
  fprintf(stderr, "usage tl_pingb -c <count> [-s <size>] [-i <iface>] <id>" \
  " <address>\n");
}

int main(int argc, char *argv[]){
  int     router;                     // Identifikator routeru
  int     result;                     // Navratovy kod funkci
  char    answer[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru
  int     parameter;                  // Rozebirani parametru
  char    command[512];               // Prikaz odesilany routeru
  int     count;                      // Pocet odesilanych pingu
  char    size[128];                  // Velikost odesilaneho pingu
  char    iface[128];                 // Interface kam je ping odeslan
  char    *address;                   // Adresa kam je ping odeslan

  // Inicializace promenych
  count = 0;
  size[0] = '\0';
  iface[0] = '\0';
  address = NULL;

  // Rozbor parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "c:s:i:")) != -1){
    switch (parameter){
      case 'c':
        count = atoi(optarg);
        break;
      case 's':
        if(atoi(optarg) > 0){
          snprintf(size, sizeof(size), "-s %s", optarg);
        }else{
          help();
          return 1;
        }
        break;
      case 'i':
        snprintf(iface, sizeof(iface), "-I %s", optarg);
        break;
      case '\?':
        help();
        return 1;
      case ':':
        help();
        return 1;
    }
  }

  // Kontrola parametru count
  if(count <= 0){
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

  // Kontrola parametru id routeru
  if(++optind >= argc){
    help();
    return 1;
  }

  // Kontrola pritomnosti parametru expression
  address = argv[optind];

  // Spusteni tcpdumpu
  sprintf(command, "ping -c %d %s %s %s > /dev/null &", count, size, iface, \
  address);

  // Odeslani zadosti remote serveru
  result = pipe_request(router, remote_process, command, answer);

  // Kontrola odpovedi
  if(result < 0){
    fprintf(stderr, "No answer from router.\n");
    return 120;
  }else if(result > 0){
    fprintf(stderr, "Error while start ping.\n");
    return result;
  }

  return 0;
}
