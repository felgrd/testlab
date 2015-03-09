#include <stdio.h>
#include <string.h>
#include <stdlib.h>     // atoi
#include <unistd.h>     // getopt
#include "cspipe.h"

/**
 * @file tl_tcpdump.c
 *
 * @brief usage tl_tcpdump [-q] [ -g <pattern>] -t <timeout> <id> <expression>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 4.3.2015
 *
 * Program tl_slog return system log of remote router. <br>
 * Example command: tl_slog 1. Answer: System log router with id 1.
 *
 * @param <id> ID of router.
 *
 * @return 0 - Answer is valid.<br>
 *         1 - Parameter is not valid.<br>
 *         2 - Timeout is expired.
 *
 * @cond
 */

#define MODE_NO          0
#define MODE_QUIET       1
#define MODE_BACKGROUND  2
#define MODE_READONLY    3

void help(void){
  fprintf(stderr, "usage tl_tcpdump [-q || -b || -r]" \
  " -t <timeout> [-i <interface>] <id> <expression>\n");
}

int main(int argc, char *argv[]){
  int     router;                     // Identifikator routeru
  int     result;                     // Navratovy kod funkci
  char    answer[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru
  int     parameter;
  char    command[512];
  char    *expression;                // Ukazatel na pravidla tcpdumpu
  char    interface[128];             // Sledovany interface
  //char    file[PATH_MAX];             // Soubor kde je ulozen log
  int     timeout;                    // Doba sledovani provozu
  int     mode;                       // Rezim programu

  // Inicializace promenych
  timeout = 0;
  mode = MODE_NO;
  interface[0] = '\0';

  // Rozbor parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "qbrt:i:")) != -1){
    switch (parameter){
      case 'q':
        mode = MODE_QUIET;
        break;
      case 'b':
        mode = MODE_BACKGROUND;
        break;
      case 'r':
        mode = MODE_READONLY;
        break;
      case 't':
        timeout = atoi(optarg);
        break;
      case 'i':
        snprintf(interface, sizeof(interface), "-i %s", optarg);
        break;
      case '\?':
        help();
        return 1;
      case ':':
        help();
        return 1;
    }
  }

  // Kontrola parametru timeout
  if(timeout <= 0 && mode != MODE_READONLY){
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

  // Kontrola pritomnosti parametru expression
  expression = ++optind < argc ? argv[optind] : NULL;

  // Preskoceni spousteni tcpdumpu v read only rezimu
  if(mode != MODE_READONLY){

    // Spusteni tcpdumpu
    sprintf(command, "tcpdump -v %s %s > /var/data/tcpdump.log 2> /dev/null &", \
    interface, expression != NULL ? expression : "");

    // Odeslani zadosti remote serveru
    result = pipe_request(router, remote_process, command, answer);

    // Kontrola odpovedi
    if(result < 0){
      fprintf(stderr, "No answer from router (start tcpdump).\n");
      return 120;
    }else if(result > 0){
      fprintf(stderr, "Tcpdump process is not start.\n");
      return result;
    }

    // Cekani na nasbirana data pozadovany cas
    sleep(timeout);

    // Ukonceni tcpdumpu
    sprintf(command, "killall tcpdump");

    // Odeslani zadosti remote serveru
    result = pipe_request(router, remote_process, command, answer);

    // Kontrola odpovedi
    if(result < 0){
      fprintf(stderr, "No answer from router. (kill tcpdump)\n");
      return 120;
    }else if(result > 0){
      fprintf(stderr, "Tcpdump process is not running.\n");
      return result;
    }
  }

  // Ukonceni v pripade behu tcpdumpu na pozadi
  if(mode == MODE_BACKGROUND){
    return 0;
  }

  // Zpracovani vysledku
  sprintf(command, "cat /var/data/tcpdump.log");

  // Odeslani zadosti remote serveru
  result = pipe_request(router, remote_process, command, answer);

  // Kontrola odpovedi
  if(result < 0){
    fprintf(stderr, "No answer from router (read tcpdump).\n");
    return 120;
  }else if(result > 0){
    fprintf(stderr, "File with tcpdump log is not found.\n");
    return result;
  }

  if(mode != MODE_QUIET){
    printf("%s", answer);
  }

  return 0;
}
