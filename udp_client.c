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
	int socketfd;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	status = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &res);
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

	char buffer[BUFFER_SIZE];
	int length = get_string(buffer, BUFFER_SIZE) + 1;

	sendto(socketfd, (void *)buffer, length, 0, res->ai_addr, res->ai_addrlen);

	struct sockaddr_storage from;
	int from_length = sizeof from;
	recvfrom(socketfd, (void *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&from, &from_length);

	printf("%s", buffer);

	close(socketfd);
	freeaddrinfo(res);
	return 0;
}

int get_string(char *s, int lim)
{
	int c, i;
	for(i = 0; i < lim-1 && (c = getchar()) != EOF; i++)
		s[i] = c;
	s[i] = '\0';

	return i;
}
