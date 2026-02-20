#include "../include/client.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

size_t send_all(int sock, const void *buffer, size_t length)
{
	size_t total_sent = 0;
	const char *ptr = buffer;

	while (total_sent < length)
	{
		ssize_t sent = send(sock, ptr + total_sent, length - total_sent, 0);

		if (sent <= 0)
		{
			return -1;
		}

		total_sent += sent;
	}

	return total_sent;
}

size_t recv_all(int sock, void *buffer, size_t length)
{
	size_t total_received = 0;
	char *ptr = buffer;

	while (total_received < length)
	{
		ssize_t received = recv(sock, ptr + total_received, length - total_received, 0);

		if (received <= 0)
		{
			return -1;
		}

		total_received += received;
	}

	return total_received;
}

void set_nickname(char* _nickname)
{
	const size_t length = strlen(_nickname);
	_nickname[length - 1] = '\0';
	strcpy(nickname, _nickname);
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
			struct Message send_message = {0};
			strcpy(send_message.nickname, nickname);
			char buff[BUFFER_CONTENT] = {0};
			const ssize_t bytes = read(STDIN_FILENO, buff, BUFFER_CONTENT);
			buff[bytes - 1] = '\0';


			strcpy(send_message.content, buff);
			if (bytes <= 0)
			{
				break;
			}

			time_t now = time(NULL);
			const struct tm* local = localtime(&now);
			char buf[BUFFER_TIME] = {0};

			strftime(buf, sizeof(buf), "%H:%M", local);
			strcpy(send_message.time, buf);
			printf("%s", send_message.time);

			send_all(sock, &send_message, sizeof(send_message));
		}

		if (poll_fds[1].revents & POLLIN)
		{
			struct Message recv_message = {0};
			if (recv_all(sock, &recv_message, sizeof(recv_message)) == -1)
				break;

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

	client_loop(sock);

	close(sock);

	return 0;
}