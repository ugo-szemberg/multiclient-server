#include "client.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

int main(void)
{
	printf("---CLIENT---\n\n Type your nickname: ");
	char nickname[30];
	fgets(nickname, sizeof(nickname), stdin);

	const int sock = init();

	logic(sock);

	close(sock);

	return 0;
}

int init(void)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		fprintf(stderr, "Error:\n");
		exit(1);
	}

	struct sockaddr_in socket_address;
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(LISTENING_PORT);

	const int inet_return_code = inet_pton(AF_INET, CONNECTION_HOST, &socket_address.sin_addr);
	if (inet_return_code != 1)
	{
		fprintf(stderr, "Error:\n");
		exit(1);
	}

	const int socket_address_length = sizeof(socket_address);
	const int connection_status = connect(sock, (struct sockaddr*)&socket_address, socket_address_length);
	if (connection_status == -1)
	{
		fprintf(stderr, "Error:\n");
		exit(1);
	}
	else
	{
		printf("Connected to the server.\n");
	}

	return sock;
}

void logic(const int sock)
{
	struct pollfd poll_fds[2];

	poll_fds[0].fd = STDIN_FILENO;
	poll_fds[0].events = POLLIN;

	poll_fds[1].fd = sock;
	poll_fds[1].events = POLLIN;

	while (1)
	{
		poll(poll_fds, 2, -1);

		if (poll_fds[0].revents & POLLIN)
		{
			char buffer[BUFFER_SIZE];
			const ssize_t size = read(STDIN_FILENO, buffer, BUFFER_SIZE);
			if (size > 0)
			{
				send(sock, buffer, size, 0);
			}
		}

		if (poll_fds[1].revents & POLLIN)
		{
			char buffer[BUFFER_SIZE];
			const ssize_t size = recv(sock, buffer, BUFFER_SIZE - 1, 0);
			if (size <= 0)
			{
				break;
			}
			buffer[size] = '\0';
			printf("%s", buffer);
		}
	}
}