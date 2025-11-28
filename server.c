#include "server.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
	printf("---SERVER---\n\n");

	int sock = socket(ADDRESS_FAMILY, SOCK_STREAM, 0);
	if (sock == -1)
	{
		close(sock);
		return -1;
	}

	struct sockaddr_in socketAddress;
	socketAddress.sin_family = ADDRESS_FAMILY;
	socketAddress.sin_port = htons(LISTENING_PORT);
	socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) == -1)
	{
		close(sock);
		return -1;
	}

	if (listen(sock, PENDING_QUEUE_MAXLENGTH) == -1)
	{
		close(sock);
		return -1;
	}

	fd_set sock_list;
	fd_set read_fds;
	struct timeval timer;
	int status = 0;

	FD_ZERO(&sock_list);
	FD_ZERO(&read_fds);
	FD_SET(sock, &sock_list);
	int fd_max = sock;

	while (1)
	{
		read_fds = sock_list;

		timer.tv_sec = TIMER_SELECT;
		timer.tv_usec = 0;

		status = select(fd_max + 1, &read_fds, NULL, NULL, &timer);
		if (status == -1)
		{
			return -1;
		}
		else if (status == 0)
		{
			printf("Waiting for connection...\n");
			continue;
		}

		for (int i = 0; i <= fd_max; ++i)
		{
			if (FD_ISSET(i, &read_fds))
			{
				if (i == sock)
				{
					accept_new_connection(sock, &sock_list, &fd_max);
				}
				else
				{
					read_from_socket(sock, i, &sock_list, fd_max);
				}
			}
		}
	}
	
	close(sock);

	return 0;
}

void accept_new_connection(int sock, fd_set* sock_list, int* fd_max)
{
	int sock_client = accept(sock, NULL, NULL);
	if (sock_client == -1)
	{
		close(sock);
		return;
	}

	FD_SET(sock_client, sock_list);
	if (sock_client > *fd_max)
	{
		*fd_max = sock_client;
	}
	printf("Server accepted new connection\n-New connection: Client %d\n", sock_client);

	char message[] = "[Server] Hello new client\n";
	if (send(sock_client, message, strlen(message), 0) == -1)
	{
		close(sock);
		return;
	}
}

void read_from_socket(int sock_server, int sock_client, fd_set* sock_list, int fd_max)
{
	char buffer[BUFFER_SIZE] = { 0 };
	if (recv(sock_client, buffer, sizeof(buffer), 0) == -1)
	{
		close(sock_server);
		return;
	}

	for (int i = 0; i <= fd_max; ++i)
	{
		if (FD_ISSET(i, sock_list) && i != sock_client && i != sock_server)
		{
			char message[BUFFER_SIZE] = { 0 };
			snprintf(message, sizeof(message), "[CLIENT %d] %s", sock_client, buffer);

			if(send(i, message, sizeof(message), 0) == -1)
			{
				close(sock_server);
				return;
			}
		}
	}
}