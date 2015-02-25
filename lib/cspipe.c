#include <stdio.h>			// snprintf
#include <fcntl.h>			// open
#include <unistd.h>			// getpid
#include <sys/stat.h>		// mkfifo
#include <string.h>
#include "cspipe.h"

int client_starting(int pid){
	char server_pipe_name[PATH_MAX + 1];
	char client_pipe_name[PATH_MAX + 1];
	int server_fd;
	int	result;

	// Cesta k servrove roure
	snprintf(server_pipe_name, sizeof(server_pipe_name), \
	SERVER_FIFO_NAME, pid);

	// Otevreni roury
	server_fd = open(server_pipe_name, O_WRONLY);
	if(server_fd == -1){
		return 0;
	}

	// Cesta ke klientske roure
	snprintf(client_pipe_name, sizeof(client_pipe_name), \
	CLIENT_FIFO_NAME, getpid());

	// Vytvoreni klientske roury
	result = mkfifo(client_pipe_name, 0777);
	if (result == -1){
		close(server_fd);
		return 0;
	}

	return server_fd;
}

void client_ending(int pid, int pipe_id){
	char server_pipe_name[PATH_MAX + 1];
	char client_pipe_name[PATH_MAX + 1];

	// Cesta k servrove roure
	snprintf(server_pipe_name, sizeof(server_pipe_name), \
	SERVER_FIFO_NAME, pid);

	// Cesta k servrove roure
	snprintf(client_pipe_name, sizeof(client_pipe_name), \
	CLIENT_FIFO_NAME, getpid());

	// Uzavreni servorove roury
	if(pipe_id != -1){
		close(pipe_id);
	}

	// Smazani servrove roury
	unlink(client_pipe_name);
}

int send_mess_to_server(int pipe_id, message_remote message){
	int write_bytes;

	// Kontrola otevreni roury
	if(pipe_id == -1){
		return 0;
	}

	// Identifikace odesilatele ve sprave
	message.client_pid = getpid();

	// Zapis dat do roury
	write_bytes = write(pipe_id, &message, sizeof(message));

	return write_bytes == sizeof(message);
}

int read_response_from_server(message_remote *message){
	char client_pipe_name[PATH_MAX + 1];
	int client_fd;
	int client_write_fd;
	int read_bytes;


	// Kontrola parametru message
	if(!message){
		return 0;
	}

	// Cesta k cteci roure
	snprintf(client_pipe_name, sizeof(client_pipe_name), \
	CLIENT_FIFO_NAME, getpid());

	// Otevreni cteci roury
	client_fd = open(client_pipe_name, O_RDONLY);
	if(client_fd == -1){
		printf("client open rdonly error\n");
		return 0;
	}

	// Otevreni cteci roury
	client_write_fd = open(client_pipe_name, O_WRONLY);
	if(client_write_fd == -1){
		printf("client open wronly error\n");
		close(client_fd);
		client_fd = -1;
		return 0;
	}

	// Cteni odpovedi
	read_bytes = read(client_fd, message, sizeof(*message));

	// Ukonceni cteni
	close(client_fd);
	close(client_write_fd);

	return read_bytes == sizeof(*message);

}

int pipe_request(int pid, client_request type, char *request, char *response){
	int             result;           // Navratova hodnota funkce
	int	          	fifo_fd;          // Roura pro prijem dat
	message_remote	remote_request;		// Format posilanych dat
	message_remote	remote_response;	// Format posilanych dat

	// Nastaveni pozadavku
	remote_request.request = type;

	// Kopirovani zadosti
	if(request != NULL){
		strncpy(remote_request.data, request, sizeof(remote_request.data));
	}

	// Navazani spojeni
	fifo_fd = client_starting(pid);
	if(fifo_fd){
		// Odelani zadosti
		result = send_mess_to_server(fifo_fd, remote_request);

		// Prijem odpovedi
		if(result){
			read_response_from_server(&remote_response);
		}
	}

	// Ukonceni komunikace
	client_ending(pid, fifo_fd);

	// Kopirovani vzsledku do bufferu
	strcpy(response, remote_response.data);

	// Kontrola validni odpovedi
	if(remote_response.request != remote_response_ok){
		return  -1;
	}

	// Navrat navratoveho kodu prikazu v testovanem zarizeni
	if(type == remote_process){
		return remote_response.result_code;
	}

	return 0;
}
