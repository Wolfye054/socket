#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>

#define SERVER_PORT "12000"
#define BUFFER_SIZE 1024

int main(void)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int status;
	int socketfd;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, SERVER_PORT, &hints, &res);
	if(status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}

	socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(socketfd == -1)
	{
		fprintf(stderr, "could not create socket\n");
		return 2;
	}

	bind(socketfd, res->ai_addr, res->ai_addrlen);
	printf("the server is ready to receive\n");

	for (;;)
	{
		char buffer[BUFFER_SIZE];

		struct sockaddr_storage from;
		int from_length = sizeof from;

		recvfrom(socketfd, (void *)buffer, BUFFER_SIZE, 0,
				(struct sockaddr *)&from, &from_length);

		printf("received message: %s\n", buffer);

		sendto(socketfd, (void *)buffer, from_length, 0,
				(struct sockaddr *)&from, from_length);
	}
}
