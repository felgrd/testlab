#include <stdio.h>
#include <string.h>
#include <stdlib.h>     //atoi
#include <sys/types.h>	// pid_t
#include <unistd.h>     // getopt
#include "cspipe.h"

#define DEFAULT_NUMBER_LINES 10

/**
 * @file tl_slog.c
 *
 * @brief usage tl_klog [-p <program>] <id>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 11.2.2015
 *
 * Program tl_klog return system log of remote router. <br>
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

void help(void){
  fprintf(stderr, "usage tl_klog [-p <program>] <id>\n");
}

int main(int argc, char *argv[]){
  int     router;                     // Identifikator routeru
  int     result;                     // Navratovy kod funkci
  char    answer[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru
  char    *line;
  char    *program;
  int     parameter;
  char    command[512];

  // Inicializace promenych
  program = NULL;

  // Rozbor parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "p:")) != -1){
    switch (parameter){
      case 'p':
        program = optarg;
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

  // Sestaveni komandu
  sprintf(command, "dmesg");

  // Odeslani zadosti remote serveru
  result = pipe_request(router, remote_process, command, answer);

  // Kontrola odpovedi
  if(result != 0){
    fprintf(stderr, "No answer from router.\r");
    return 1;
  }

  // Prvni radek
  line = strtok(answer, "\n");

  // Prochazeni logem
  while(line != NULL){

    // Vypis urcitych radku logu
    if(program == NULL || strstr(line, program) != NULL){
      printf("%s", line);
    }

    // Dalsi radek logu
    line = strtok(NULL, "\r");
  }

  // Ukonceni s navratovou hodnotou z remote serveru
  return !result;
}
