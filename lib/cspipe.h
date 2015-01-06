#ifndef __CSPIPE_H
#define __CSPIPE_H

#include <sys/types.h>
#include <limits.h>

#define SERVER_FIFO_NAME	"/tmp/remote_%d_pipe"
#define CLIENT_FIFO_NAME	"/tmp/remote_%d_pipe"

#define PIPE_BUFFER_SIZE	8192

typedef enum{
	remote_process = 0,				// Vykonani prikazys
	remote_change_address,			// Zmena adresy pripojeni
	remote_change_port,				// Zmena portu pripojeni
	remote_change_user,				// Zmena prihlasovaciho jmena
	remote_change_pass,				// Zmena prihlasovaciho hesla
	remote_change_protocol,			// Zmena protokolu pro prihlaseni
	remote_default_conf,			// Nastaveni defaultni konfigurace
	remote_reconnect,				// Znovu pripojeni do routeru
	remote_close_conn,				// Ukonceni aplikace
	remote_status_address,			// Informace o aktualni adrese
	remote_status_port,				// Informace o aktualnim portu
	remote_status_user,				// Informace o aktualnim uzivateli
	remote_status_pass,				// Informace o aktualnim hesle
	remote_status_protocol,			// Informace o aktualnim prihlaovacim protokolu	
	remote_response_ok,				// Uspesne vykonani
	remote_response_error			// Chyba ve skriptu
}client_request;


typedef struct message_remote {
	pid_t			client_pid;
	client_request	request;
	char			data[PIPE_BUFFER_SIZE];
}message_remote;


int client_starting(int pid);
void client_ending(int pid, int pipe_id);
int send_mess_to_server(int pipe_id, message_remote message);

int read_response_from_server(message_remote *message);

#endif
