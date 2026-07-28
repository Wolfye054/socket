#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define SERVER_PORT "12000"
#define BUFFER_SIZE 1024
#define HTTP_404 "HTTP/1.0 404 Not Found\r\n"
#define HTTP_200 "HTTP/1.0 200 OK\r\n\r\n"

char *get_filename(char *s, int lim);

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

	//TODO: error checking?
	bind(server_socket, res->ai_addr, res->ai_addrlen);
	listen(server_socket, 1);

	for(;;)
	{
		printf("ready to serve...\n");

		char buffer[BUFFER_SIZE];
		struct sockaddr_storage from;
		int from_size = sizeof from;

		int connection_socket = accept(server_socket, (struct sockaddr *)&from, &from_size);
		recv(connection_socket, (void *)buffer, BUFFER_SIZE, 0);

		char *filename = get_filename(buffer, BUFFER_SIZE);
		FILE *file = fopen(filename, "rb");

		printf("%s\n", filename);

		if(!file)
		{
			send(connection_socket, (void *)HTTP_404, sizeof HTTP_404 - 1, 0);
			continue;
		}

		send(connection_socket, (void *)HTTP_200, sizeof HTTP_200 - 1, 0);

		for(void *data; fread(data, 1, 1, file);)
		{
			send(connection_socket, data, 1, 0);
		}
		send(connection_socket, (void *)"\r\n", sizeof "\r\n" - 1, 0);

		close(connection_socket);
	}
}

char *get_filename(char *s, int lim)
{
	static char buffer[BUFFER_SIZE];
	int i, name_length;

	buffer[0] = '\0';

	for(i = 0; i < lim && isspace(s[i]); i++)
		;

	for(; i < lim && isalpha(s[i]); i++)
		;

	for(name_length = 0; i < lim && !isspace(s[i]); i++, name_length++)
		buffer[name_length] = s[i];

	return buffer;
}
