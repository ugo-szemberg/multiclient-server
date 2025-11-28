#include "server.h"

int main(void)
{
	printf("---SERVER---\n\n");
	init();

	SOCKET sock = socket(ADDRESS_FAMILY, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		return_error();
	}

	struct sockaddr_in socketAddress;
	socketAddress.sin_family = ADDRESS_FAMILY;
	socketAddress.sin_port = htons(LISTENING_PORT);
	socketAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(sock, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR)
	{
		closesocket(sock);
		return_error();
	}

	if (listen(sock, PENDING_QUEUE_MAXLENGTH) == SOCKET_ERROR)
	{
		closesocket(sock);
		return_error();
	}

	fd_set sock_list;
	fd_set read_fds;
	struct timeval timer;
	int status = 0;

	FD_ZERO(&sock_list);
	FD_ZERO(&read_fds);
	FD_SET(sock, &sock_list);
	int fd_max = (int)sock;

	while (1)
	{
		read_fds = sock_list;

		timer.tv_sec = TIMER_SELECT;
		timer.tv_usec = 0;

		status = select(fd_max + 1, &read_fds, NULL, NULL, &timer);
		if (status == SOCKET_ERROR)
		{
			return_error();
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
				if ((SOCKET)i == sock)
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
	
	closesocket(sock);
	close();

	return 0;
}

void return_error(void)
{
	fprintf(stderr, "Error: %d\n", WSAGetLastError());
	WSACleanup();
	exit(1);
}

void init(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}
}

void close(void)
{
	WSACleanup();
}

void accept_new_connection(SOCKET sock, fd_set* sock_list, int* fd_max)
{
	SOCKET sock_client = accept(sock, NULL, NULL);
	if (sock_client == INVALID_SOCKET)
	{
		return_error();
	}

	FD_SET(sock_client, sock_list);
	if (sock_client > *fd_max)
	{
		*fd_max = (int)sock_client;
	}
	printf("Server accepted new connection\n-New connection: Client %d\n", (int)sock_client);

	char message[] = "[Server] Hello new client\n";
	if (send(sock_client, message, (int)strlen(message), 0) == SOCKET_ERROR)
	{
		return_error();
	}
}

void read_from_socket(SOCKET sock_server, SOCKET sock_client, fd_set* sock_list, int fd_max)
{
	char buffer[BUFFER_SIZE] = { 0 };
	if (recv(sock_client, buffer, sizeof(buffer), 0) == SOCKET_ERROR)
	{
		return_error();
	}

	for (int i = 0; i <= fd_max; ++i)
	{
		if (FD_ISSET(i, sock_list) && i != sock_client && i != sock_server)
		{
			char message[BUFFER_SIZE] = { 0 };
			snprintf(message, sizeof(message), "[CLIENT %d] %s", (int)sock_client, buffer);

			if(send(i, message, sizeof(message), 0) == SOCKET_ERROR)
			{
				return_error();
			}
		}
	}
}