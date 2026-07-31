#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SERVER_PORT "12000"
#define BUFFER_SIZE 1024

int to_upper(char *s);

int main(void)
{
	srand(time(NULL));

	struct addrinfo hints = {0};
	struct addrinfo *res;
	int status;
	int server_socket;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, SERVER_PORT, &hints, &res);
	if(status != 0)
	{
		printf("could not get addrinfo\n");
		return 1;
	}

	server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(server_socket == -1)
	{
		printf("could not open socket\n");
		return 1;
	}

	bind(server_socket, res->ai_addr, res->ai_addrlen);
	printf("the server is ready to receive\n");

	for(;;)
	{
		int min = 1;
		int max = 10;
		int random = (rand() % (max - min + 1) + min);

		char buffer[BUFFER_SIZE];
		struct sockaddr_storage from;
		socklen_t from_length = sizeof from;

		int size = recvfrom(server_socket, (void *)buffer, BUFFER_SIZE, 0,
				(struct sockaddr *)&from, &from_length);

		buffer[size] = '\0';
		to_upper(buffer);

		if(random < 4)
		{
			continue;
		}

		sendto(server_socket, (void *)buffer, size, 0, (struct sockaddr *)&from, from_length);
	}
}

int to_upper(char *s)
{
	int c, i;
	for(i = 0; (c = s[i]) != '\0'; i++)
		if(c >= 'a' && c <= 'z')
			s[i] = c - 32;
	return i;
}
