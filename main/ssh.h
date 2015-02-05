#ifndef __SSH_H
#define __SSH_H

// Default TELNET login data
#define SSH_PORT 23

// Size of buffers
#define SSH_STR_BUFFER 8192
#define SSH_RCV_BUFFER 1024
#define SSH_TR_BUFFER  1024

// Timeouts
#define SSH_CONNECT_TIMEOUT 30
#define	SSH_RECEIVE_TIMEOUT 20

int pipes_stdin[2];     // Roura pro komunikaci s PLINK
int pipes_stdout[2];    // Roura pro komunikaci s PLINK

/**
 * Inicializace ssh spojeni
 */
int sshInit(char *ip, int port, const char *username, const char *password);

/**
 * Kontrola behu ssh deamona PLINK
 */
int sshCheck(pid_t pid);

/**
 * Ukonceni ssh deamona PLINK
 */
int sshDone(pid_t pid);

/**
 * Odeslani prikazu protokolem ssh pres demona PLINK
 */
int sshExec(pid_t pid, char *request, char *response, int length);

#endif
