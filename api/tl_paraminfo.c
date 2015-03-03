#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>     //atoi
#include <sys/types.h>	// pid_t
#include <unistd.h>     // getopt
#include "cspipe.h"

/**
* @file tl_paraminfo.c
*
* @brief usage tl_paraminfo -f \<function\> -p \<param\> [-r \<profile\>]
* \<id\>
*
* @author David Felgr
* @version 1.0.0
* @date 11.2.2015
*
* Program tl_paraminfo informs about configuration parameter in router. <br>
* Example command: tl_paraminfo -f ppp -p apn 1.
* Answer: conel.agnep.cz.
*
* @param <id> ID of router.
*
* @return 0 - Answer is valid.<br>
*         1 - Parameter is not valid.<br>
*         2 - Comunicate with router error.<br>
*         3 - Change parameter error.
*
* @cond
*/

void help(void){
  fprintf(stderr, "usage tl_paramchange -f <function> -p <param>" \
  "[-r <profile>] <id> \n");
}

char *toUpperCase(char *text){
  int i = 0;

  while(text[i]){
    text[i] = toupper(text[i]);
    i++;
  }

  return text;
}

char *toLowerCase(char *text){
  int i = 0;

  while(text[i]){
    text[i] = tolower(text[i]);
    i++;
  }

  return text;
}

int main(int argc, char *argv[]){
  int     router;                     // Identifikator routeru
  int     result;                     // Navratovy kod funkci
  char    answer[PIPE_BUFFER_SIZE];   // Buffer pro opoved z routeru
  char    command[512];               // Prikaz pro zmenu parametru
  int     options;                    // Promena pro rozebirani parametru
  char    *function;                  // Funkce u ktere bude zmenen parametr
  char    *parameter;                 // Parametr ktery bude menen
  char    *profile;                   // Profil kde bude parametr menen
  char    *function_upper;            // Funkce prevedena na velka pismena
  char    *function_lower;            // Funkce prevedena na mala pismena

  // Inicializace promenych
  function = NULL;
  parameter = NULL;
  profile = NULL;

  // Rozbor parametru na prikazove radce
  while ((options = getopt(argc, argv, "f:p:r:")) != -1){
    switch (options){
      case 'f':
        function = optarg;
        break;
      case 'p':
        parameter = optarg;
        break;
      case 'r':
        profile = optarg;
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
  if(function == NULL){
    help();
    return 1;
  }

  // Kontrola parametru parameter
  if(parameter == NULL){
    help();
    return 1;
  }

  if(profile == NULL){
    profile = strdup("");
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

  // Prevedeni funkce na velke pismena
  parameter = toUpperCase(parameter);

  // Prevedeni funkce na velke pismena
  function_upper = strdup(function);
  function_upper = toUpperCase(function_upper);

  // prevedeni funkce na mala psimena
  function_lower = strdup(function);
  function_lower = toLowerCase(function_lower);

  // Sestaveni komandu
  sprintf(command, "grep %s_%s= /etc/%s/settings.%s", function_upper, \
  parameter, profile, function_lower);

  // Odeslani zadosti remote serveru
  result = pipe_request(router, remote_process, command, answer);

  // Uvolneni pameti
  free(function_upper);
  free(function_lower);

  // Kontrola odpovedi
  if(result < 0){
    fprintf(stderr, "Comunaction error: %s\n", answer);
    return 2;
  }else if(result > 0){
    fprintf(stderr, "Error with search item %s in configuration %s.\n", \
    parameter, function);
    return 3;
  }

  // Tisk odpovedi
  printf("%s", strchr(answer, '=') + 1);

  return 0;
}
