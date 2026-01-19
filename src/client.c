#include "client.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>

int main(void)
{
	printf("---CLIENT---\n\n Type your nickname: ");
	char nickname[BUFFER_NICKNAME];
	if (!fgets(nickname, BUFFER_NICKNAME, stdin))
	{
		return -1;
	}

	set_nickname(nickname);

	const int sock = connect_to_server();

	client_loop(sock);

	close(sock);

	return 0;
}

void set_nickname(const char* nickname)
{
	const size_t length = strcspn(nickname, "\n");
	memcpy(message.nickname, nickname, length);
	message.nickname[length] = '\0';
}

void reset_message_content(void)
{
	memset(message.content, '\0', BUFFER_MESSAGE);
}

int connect_to_server(void)
{
	const int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		return -1;
	}

	struct sockaddr_in socket_address;
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(LISTENING_PORT);

	if (inet_pton(AF_INET, CONNECTION_HOST, &socket_address.sin_addr) != 1)
	{
		close(sock);
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&socket_address, sizeof(socket_address)) == -1)
	{
		close(sock);
		return -1;
	}
	printf("Connected to the server.\n");

	return sock;
}

void client_loop(const int sock)
{
	struct pollfd poll_fds[2];

	poll_fds[0].fd = STDIN_FILENO;
	poll_fds[0].events = POLLIN;

	poll_fds[1].fd = sock;
	poll_fds[1].events = POLLIN;

	while (1)
	{
		if (poll(poll_fds, 2, -1) == -1)
		{
			break;
		}

		if (poll_fds[0].revents & POLLIN)
		{
			const ssize_t bytes = read(STDIN_FILENO, &message.content, BUFFER_MESSAGE);
			if (bytes <= 0)
			{
				break;
			}

			send(sock, &message, sizeof(message), 0);
		}

		if (poll_fds[1].revents & POLLIN)
		{
			char buffer[BUFFER_MESSAGE];
			const ssize_t size = recv(sock, buffer, BUFFER_MESSAGE - 1, 0);
			if (size <= 0)
			{
				break;
			}

			buffer[size] = '\0';
			printf("%s", buffer);
		}
	}
}