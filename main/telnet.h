#ifndef __TELENT_H
#define __TELNET_H

// Default TELNET login data
#define TELNET_PORT	23
#define TELNET_USER	"root"
#define TELNET_PASS	"root"

// Commands
#define TELNET_IAC				0xFF
#define TELNET_DONT				0xFE
#define TELNET_DO				0xFD
#define TELNET_WANT				0xFC
#define TELNET_WILL				0xFB
#define TELNET_SUBOBTION		0xFA
#define TELNET_SUBOBTION_END	0xF0

// Subcommands
#define ECHO							0x01
#define REMOTE_FLOW_CONTROL				0x21
#define SUPRESS_GO_AHEAD				0x03
#define NEGOATIATE_ABOUT_WINDOW_SIZE	0x1F

// Characters
#define	CR	0x0D
#define LF	0x0A

// Size of buffers
#define TELNET_STR_BUFFER	8192
#define TELNET_RCV_BUFFER	1024
#define TELNET_TR_BUFFER	1024

/**
 * Inicializace spojeni
 */
int telnetInit(char *ip, int port, const char *username, const char *password);

/**
 * Ukonceni spojeni
 */
int telnetDone(int sockfd);

/**
 * Odeslani prikazu protokolem Telnet
 */
int telnetExec(int sockfd, char *request, char *response, int length);

/**
 * Odeslani dat do TCP socketu
 */
int telnetSend(int sockfd, char *buffer, int length);

/**
 * Prijem ocekavanych dat z TCP socket
 */
int telnetExpect(int sockfd, char *buffer);

#endif
