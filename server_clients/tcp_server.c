#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

#define SERVER_PORT "12000"
#define BUFFER_SIZE 1024

int to_upper(char *s);

int main(void)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int status;
	int server_socket;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(0, SERVER_PORT, &hints, &res);
	if(status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}

	server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(server_socket == -1)
	{
		fprintf(stderr, "could not create socket\n");
		return 2;
	}

	// TODO: error checking?
	bind(server_socket, res->ai_addr, res->ai_addrlen);
	listen(server_socket, 10);

	printf("the server is ready to receive\n");

	for(;;)
	{
		char buffer[BUFFER_SIZE];

		struct sockaddr_storage from;
		int from_size = sizeof from;

		int connection_socket = accept(server_socket, (struct sockaddr *)&from, &from_size);
		recv(connection_socket, (void *)buffer, BUFFER_SIZE, 0);
		int buffer_length = to_upper(buffer);
		send(connection_socket, (void *)buffer, buffer_length, 0);
		close(connection_socket);
	}

	close(server_socket);
	return 0;
}

int to_upper(char *s)
{
	int c, i;
	for(i = 0; (c = s[i]) != '\0'; i++)
		if(c >= 'a' && c <= 'z')
			s[i] = c - 32;
	return i;
}
