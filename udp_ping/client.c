#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "../ip.h"

#define SERVER_PORT "12000"
#define BUFFER_SIZE 1024

int main(void)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int status;
	int client_socket;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	status = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &res);
	if(status != 0)
	{
		printf("could not get addrinfo\n");
		return 1;
	}

	client_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(client_socket == -1)
	{
		printf("could not open socket\n");
		return 1;
	}

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	if(setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv) < 0)
	{
		printf("could not set socket option\n");
	}


	for(int i = 0; i < 10; i++)
	{
		char buffer[BUFFER_SIZE] = "\0";
		int timeval = time(NULL);
		int length = snprintf(buffer, BUFFER_SIZE, "Ping %d %d", i + 1, timeval);
		struct timespec start, end;

		clock_gettime(CLOCK_MONOTONIC, &start);

		sendto(client_socket, (void *)buffer, length, 0, res->ai_addr, res->ai_addrlen);

		struct sockaddr_storage from;
		socklen_t from_length = sizeof from;
		length = recvfrom(
				client_socket, (void *)buffer, BUFFER_SIZE - 1, 0,
				(struct sockaddr *)&from, &from_length);

		clock_gettime(CLOCK_MONOTONIC, &end);

		if(length > 0)
		{
			buffer[length] = '\0';

			double elapsed_ms =
				(end.tv_sec - start.tv_sec) * 1000.0 + 
				(end.tv_nsec - start.tv_nsec) / 1000000.0;

			printf("%s\n", buffer);
			printf("RTT: %.3f ms\n\n", elapsed_ms);
		}
		else
		{
			printf("request %d timed out\n\n", i + 1);
		}
	}
}
