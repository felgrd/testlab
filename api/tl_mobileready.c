#include <stdio.h>
#include <stdlib.h>
#include <time.h>      // time
#include <unistd.h>    // getopt
#include <string.h>
#include "cspipe.h"

// Defaultni cas zkouseni stavu routeru
#define DEFUALT_TIMEOUT		120

/**
* @file tl_mobileready.c
*
* @brief usage tl_mobileready [-t <timeout>] <id>
*
* @author David Felgr
* @version 1.0.0
* @date 11.2.2015
*
* Program tl_moblieready wait router is connected to moblie network. <br>
* Example command: tl_mobileready 1. Answer: 20. Program wait 20 seconds.
*
* @param <id> ID of router.
*
* @return 0 - Answer is valid.<br>
*         1 - Parameter is not valid.<br>
*         2 - Timeout is expired.
*
* @cond
*/

#define STATE_DISCONNECT     3
#define STATE_TIMEOUT        2
#define STATE_CONNECT        0

void help(void){
  printf("usage usage tl_mobileready [-t <timeout>] <id>\n");
}

int main(int argc, char *argv[]){
  int     timeout;            // Cas maximalniho cekani na odpoved routeru
  int     starttime;          // Cas spusteni programu
  int     router;             // Identifikator routeru
  int     parameter;          // Parametr prichazejici na prikazove radcec
  int     result;
  int     state;              // Stav testovani
  char    request[PIPE_BUFFER_SIZE];    // Buffer pro opoved z routeru
  char    response[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru

  // Inicializace promenych
  state = STATE_DISCONNECT;
  timeout = DEFUALT_TIMEOUT;

  // Rozebirani parametru na prikazove radce
  while ((parameter = getopt(argc, argv, "t:")) != -1) {
    switch (parameter) {
      case 't':
        timeout = atoi(optarg);
        if(timeout <= 0){
          timeout = DEFUALT_TIMEOUT;
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

  // Zacatek kontroly stavu routeru
  starttime = time(NULL);

  while(state == STATE_DISCONNECT){

    // Vytvoreni zpravy
    snprintf(request, sizeof(request), "status ppp | grep IP");

    // Odeslani zadosti remote serveru
    result = pipe_request(router, remote_process, request, response);

    // Kontrola timeoutu
    if(starttime + timeout < time(NULL)){
      state = STATE_TIMEOUT;
      fprintf(stderr, "Timeout expired while router connect to mobile network.\n");
    }

    // Kontrola odpovedi od routeru
    if(result == 0 && strstr(response, "Unassigned") == NULL){
      state = STATE_CONNECT;
    }else{
      sleep(1);
    }
  }

  // Tisk celkove doby cekani na nabehnuti routeru
  printf("%d\n", (int)(time(NULL) - starttime));

  return state;
}
