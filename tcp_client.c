#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include "ip.h"

#define SERVER_PORT "12000"
#define BUFFER_SIZE 1024

int get_string(char *s, int lim);

int main(void)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int status;
	int client_socket;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &res);
	if(status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}

	client_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(client_socket == -1)
	{
		fprintf(stderr, "could not create socket\n");
		return 2;
	}

	status = connect(client_socket, res->ai_addr, res->ai_addrlen);
	if(status != 0)
	{
		fprintf(stderr, "could not connect socket\n");
		return 3;
	}

	char buffer[BUFFER_SIZE];
	int length = get_string(buffer, BUFFER_SIZE) + 1;

	send(client_socket, (void *)buffer, length, 0);
	recv(client_socket, (void *)buffer, BUFFER_SIZE, 0);

	printf("from server: %s\n", buffer);
	close(client_socket);
}

int get_string(char *s, int lim)
{
	int c, i;
	for(i = 0; i < lim-1 && (c = getchar()) != EOF; i++)
		s[i] = c;
	s[i] = '\0';

	return i;
}
