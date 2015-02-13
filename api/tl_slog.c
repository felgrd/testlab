#include <stdio.h>

/**
 * @file tl_slog.c
 *
 * @brief usage tl_slog <id>
 *
 * @author David Felgr
 * @version 1.0.0
 * @date 11.2.2015
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

void help(void){
  printf("usage tl_slog [-n <lines>] [-p <program>] <id>\n");
}

int main(int argc, char *argv[]){




  return 0;
}
