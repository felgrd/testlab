#ifndef __CSPIPE_H
#define __CSPIPE_H

#include <limits.h>

/**
 * @file cspipe.h
 */

/// Name of fifo for read communication with remote server
#define SERVER_FIFO_NAME	"/tmp/remote_%d_pipe"
/// Name of fifo for write communication with remote server
#define CLIENT_FIFO_NAME	"/tmp/remote_%d_pipe"
/// Max size of data write or read from pipe
#define PIPE_BUFFER_SIZE	8192

typedef enum{
	remote_process = 0,       ///< Vykonani prikazys
	remote_change_address,    ///< Zmena adresy pripojeni
	remote_change_port,       ///< Zmena portu pripojeni
	remote_change_user,       ///< Zmena prihlasovaciho jmena
	remote_change_pass,       ///< Zmena prihlasovaciho hesla
	remote_change_protocol,   ///< Zmena protokolu pro prihlaseni
	remote_default_conf,      ///< Nastaveni defaultni konfigurace
	remote_reconnect,         ///< Znovu pripojeni do routeru
	remote_close_conn,        ///< Ukonceni aplikace
	remote_status_address,    ///< Informace o aktualni adrese
	remote_status_port,       ///< Informace o aktualnim portu
	remote_status_user,       ///< Informace o aktualnim uzivateli
	remote_status_pass,       ///< Informace o aktualnim hesle
	remote_status_protocol,   ///< Informace o aktualnim prihlaovacim protokolu
	remote_response_ok,       ///< Uspesne vykonani
	remote_response_error     ///< Chyba ve skriptu
}client_request;

/**
 * @brief Message structure
 */
typedef struct message_remote {
	pid_t			client_pid;               ///< Id of router
	client_request	request;                  ///< Type of request
	char			data[PIPE_BUFFER_SIZE];   ///< Buffer for data
}message_remote;

/**
 * @brief Open communication with remote server
 *
 * @param pid Router id of your tested router.
 * @return If start communication is access, function return file
 * descriptor. Else function return zero.
 */
int client_starting(int pid);

/**
 * @brief Close communication with remote server
 *
 * @param pid Router id of your tested router.
 * @param pipe_id File descriptor of actual communication.
 */
void client_ending(int pid, int pipe_id);

/**
 * @brief Send message to remote server
 *
 * @param pipe_id File descriptor of actual communication.
 * @param message Sended message.
 * @return Return 1 - if it sended full message, else return 0.
 */
int send_mess_to_server(int pipe_id, message_remote message);

/**
 * @brief Read response from remote server
 *
 * @param message Pointer to buffer for save answer.
 * @return Return 1 - if it readed full message, else return 0.
 */
int read_response_from_server(message_remote *message);

#endif
