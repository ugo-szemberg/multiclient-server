#include "../include/network.h"
#include "../include/message.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

void set_nickname(char* nickname)
{
	const size_t length = strlen(nickname);
	nickname[length - 1] = '\0';
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

void client_loop(const int sock, const char* nickname)
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
			Message send_message = {0};
			strcpy(send_message.nickname, nickname);
			const ssize_t bytes = read(STDIN_FILENO, &send_message.content, BUFFER_CONTENT);
			if (bytes <= 0)
			{
				break;
			}
			send_message.content[bytes - 1] = '\0';

			time_t now = time(NULL);
			const struct tm* local = localtime(&now);
			strftime(send_message.time, sizeof(send_message.time), "%H:%M", local);

			send_all(sock, &send_message, sizeof(send_message));
		}

		if (poll_fds[1].revents & POLLIN)
		{
			Message recv_message = {0};
			if (recv_all(sock, &recv_message, sizeof(recv_message)) == -1)
			{
				break;
			}

			printf("[%s] %s %s\n", recv_message.nickname, recv_message.content, recv_message.time);
		}
	}
}

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

	client_loop(sock, nickname);

	close(sock);

	return 0;
}