#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>		// inet_pton
#include <unistd.h>			// close
#include <string.h>			// memset, strcat, strcpy
#include "telnet.h"

int telnetInit(char *ip, int port, const char *username, const char *password){
	int sockfd;	
	int i;
	int ok;
	struct sockaddr_in addr;	
	char buffer[256];
	int buffer_length;	
	char trBuffer[256];
	int trBufferLength;	
	char c1, c2;
	
	// Inicializace promenych
	buffer_length = 0;
	trBufferLength = 0;
	
	// Vytvoreni socketu
	if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		return 0;
	}
	
	// Zpracovani adresacnich udaju
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(TELNET_PORT);
	
	// Prevod IP adresy
	ok = inet_pton(AF_INET, ip, &addr.sin_addr);	
	if (ok <= 0) {
		return 0;
	}
	
	// Vytvoreni TCP spojeni
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof addr) < 0){
		close(sockfd);
		return 0;
	}
	
	// Vyjednavani parametru
	do {	
		buffer_length = recv(sockfd, buffer, sizeof(buffer), 0);
			
		ok = strstr(buffer, "login") != NULL;
		
		if(buffer_length == 0) {
			break;
		}
		
		if(!ok){			
			for(i = 0; i < buffer_length; i++){
				c1 = buffer[i++];
				if(c1 == TELNET_IAC){
					c1 = buffer[i++];
					c2 = buffer[i];
					if( c1 == TELNET_DO){
							trBuffer[trBufferLength++] = TELNET_IAC;
							trBuffer[trBufferLength++] = TELNET_WANT;
							trBuffer[trBufferLength++] = c2;
					}										
				}
			}
			if(trBufferLength > 0){
				telnetSend(sockfd, trBuffer, trBufferLength);
			}
		}
	}while(!ok);
	
	// Logovani - odeslani username
	if(ok){		
		strcpy(trBuffer, username);
		strcat(trBuffer, "\r\n");
		trBufferLength = strlen(trBuffer);
		telnetSend(sockfd, trBuffer, trBufferLength);		
		ok = telnetExpect(sockfd, "Password:");		
	}
	
	// Logovani - odeslani password	
	if(ok){
		strcpy(trBuffer, password);
		strcat(trBuffer, "\r\n");
		trBufferLength = strlen(trBuffer);
		telnetSend(sockfd, trBuffer, trBufferLength);
		ok = telnetExpect(sockfd, "#");
	}
	
	// Ukonceni spojeni v pripade neuspesneho prihlaseni
	if(!ok){
		telnetDone(sockfd);
		sockfd = 0;
	}
	
	return sockfd;
	
}

int telnetSend(int sockfd, char *buffer, int length){

	return send(sockfd, buffer, length, MSG_NOSIGNAL);
}

int telnetExpect(int sockfd, char *buffer){
	int i;
	int rcvLength;
	char rcvBuffer[256];
	char strBuffer[1024];
	struct timeval tv;

	// Nastaveni timeoutu pro prijem dat
	tv.tv_sec = 5; 
	tv.tv_usec = 0; 
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	
	// Cekani na prijem pozadovanych dat
	for (i = 0; i < 4; i++){
		rcvLength = recv(sockfd, rcvBuffer, sizeof(rcvBuffer), 0);	
		if(rcvLength < 0){
			return 0;
		}		
		strcat(strBuffer, rcvBuffer);
		if(strstr(strBuffer, buffer) != NULL){
			return 1;
		}
	}
	
	return 0;
}

int telnetDone(int sockfd){
	if(sockfd > 0){
		if(close(sockfd) != 0){
			return 0;
		}
		return 1;
	}
	return 0;
}

int telnetExec(int sockfd, char *request, char *response, int length){
	char	strBuffer[TELNET_STR_BUFFER];
	char	rcvBuffer[TELNET_RCV_BUFFER];
	char	trBuffer[TELNET_TR_BUFFER];	
	int		rcvLength;
	int		trLength;
	int		rspLength;
	char	*start;
	char	*stop;	
	struct timeval tv;
	
	// Inicializace bufferu
	strBuffer[0] = '\0';
	
	rcvLength = 0;
	trLength = 0;
	
	
	// Kontrola platnosti socketu
	if(sockfd <= 0){
		strcpy(response, "Telnet error: Connection is not established.");
		return 0;
	}

	// Nastaveni timeoutu pro prijem dat
	tv.tv_sec = 1; 
	tv.tv_usec = 0; 
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
		
	// Odeslani prikazu	
	strcpy(trBuffer, request);
	strcat(trBuffer, "\r\n");
	trLength = telnetSend(sockfd, trBuffer, strlen(trBuffer));
	if(trLength != strlen(trBuffer)){
		return 0;
	}
	
	// Prijem odpovedi
	do {
		rcvLength = recv(sockfd, rcvBuffer, sizeof(rcvBuffer), 0);	
		
		if(rcvLength < 0){
			strcpy(response, "Telnet error: No data is received.");
			return 0;
		}
		
		if(strlen(strBuffer) + rcvLength > sizeof(strBuffer)){
			strcpy(response, "Telnet error: Output si too large.");
			return 0;
		}
		
		rcvBuffer[rcvLength] = '\0';			
		strncat(strBuffer, rcvBuffer, sizeof(strBuffer));				
	} while(strstr(rcvBuffer, "#") == NULL);
	
	// Zpracovani odpovedi
	start = strstr(strBuffer, trBuffer);
	stop = strstr(strBuffer, "#");
	
	if(start == NULL || stop == NULL) {
		strcpy(response, "Telnet error: Start or stop is not find.");
		return 0;
	}
	
	start += trLength;
	
	if(start == stop) {
		strcpy(response, "Telnet error: No answer.");
		return 0;
	}
	
	if(start > stop) {
		strcpy(response, "Telnet error: Answer is not valid.");
		return 0;
	}
	
	// Kontrola velikosti odpovedi
	rspLength = stop - start;	
	if(rspLength > length){
		rspLength = length;
	}
	
	strncpy(response, start, stop - start);
	response[stop - start] = '\0';
	
	return 1;	
}
	
