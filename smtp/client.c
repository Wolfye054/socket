#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../ip.h"

#define SERVER_PORT "1025"
#define BUFFER_SIZE 1024

int get_response_code(char *s);
int recv_string(int socket, char *s, int lim);
void send_string(int socket, char *s);

int main(void)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	int status;
	int client_socket;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &res);
	if(status)
	{
		printf("could not get address information\n");
		return 1;
	}

	client_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(!client_socket)
	{
		printf("could not connect to socket\n");
		return 1;
	}

	status = connect(client_socket, res->ai_addr, res->ai_addrlen);
	if(status == -1)
	{
		printf("could not connect\n");
		return 1;
	}

	char buffer[BUFFER_SIZE] = "\0";
	int response_code;

	recv_string(client_socket, buffer, BUFFER_SIZE);
	response_code = get_response_code(buffer);
	if(response_code != 220)
	{

		printf("220 reply not received from server\n");
	}

	send_string(client_socket, "HELO Alic\r\n");

	recv_string(client_socket, buffer, BUFFER_SIZE);
	response_code = get_response_code(buffer);
	if(response_code != 250)
	{
		printf("250 reply not received from server\n");
	}

	send_string(client_socket, "MAIL FROM:<bob@example.org>\r\n");

	recv_string(client_socket, buffer, BUFFER_SIZE);
	response_code = get_response_code(buffer);
	if(response_code != 250)
	{
		printf("250 reply not received from server\n");
	}

	send_string(client_socket, "RCPT TO:<alice@example.com>\r\n");

	recv_string(client_socket, buffer, BUFFER_SIZE);
	response_code = get_response_code(buffer);
	if(response_code != 250)
	{
		printf("250 reply not received from server\n");
	}

	send_string(client_socket, "DATA\r\n");

	recv_string(client_socket, buffer, BUFFER_SIZE);
	response_code = get_response_code(buffer);
	if(response_code != 354)
	{
		printf("354 reply not received from server\n");
	}

	send_string(client_socket,
			"From: \"Bob Example\" <bob@example.org>\r\n"
			"To: \"Alic Example\" <alice@example.org>\r\n"
			"Subject: Test message\r\n"
			"Hello Alice.\r\n"
			"This is a test message with 3 header fields\r\n"
			"Your friend,\r\n"
			"Bob\r\n"
			".\r\n");

	recv_string(client_socket, buffer, BUFFER_SIZE);
	response_code = get_response_code(buffer);
	if(response_code != 250)
	{
		printf("250 reply not received from server\n");
	}

	send_string(client_socket, "QUIT\r\n");
}

int recv_string(int socket, char *s, int lim)
{
	int message_length = recv(socket, s, lim-1, 0);
	s[message_length] = '\0';
	printf("%s\n", s);
	return message_length;
}

void send_string(int socket, char *s)
{
	int length;
	for(length = 0; s[length] != '\0'; length++)
		;
	send(socket, s, length, 0);
}

int get_response_code(char *s)
{
	char response_code_buffer[4];
	memcpy(response_code_buffer, s, 3);
	response_code_buffer[3] = '\0';
	return atoi(response_code_buffer);
}
