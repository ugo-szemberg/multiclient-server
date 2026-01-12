#include "server.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

int main(void)
{
	printf("---SERVER---\n\n");

	int sock = socket(ADDRESS_FAMILY, SOCK_STREAM, 0);
	if (sock == -1)
	{
		close(sock);
		return -1;
	}

	struct sockaddr_in socket_address;
	socket_address.sin_family = ADDRESS_FAMILY;
	socket_address.sin_port = htons(LISTENING_PORT);
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr*)&socket_address, sizeof(socket_address)) == -1)
	{
		close(sock);
		return -1;
	}

	if (listen(sock, PENDING_QUEUE_MAXLENGTH) == -1)
	{
		close(sock);
		return -1;
	}

	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		perror("epoll_create1");
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev) == -1) {
		perror("epoll_ctl: server_fd");
		return -1;
	}

	int clients[MAX_CLIENTS];
	int num_clients = 0;

	while (1)
	{
		struct epoll_event events[MAX_EVENTS];
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		for (int i = 0; i < nfds; i++)
		{
			int fd = events[i].data.fd;
			if (fd == sock)
				{
				accept_new_connection(sock, epoll_fd, clients, &num_clients);
			}
			else
			{
				read_from_socket(fd, epoll_fd, clients, num_clients);
			}
		}
	}
	
	close(sock);

	return 0;
}

void accept_new_connection(int sock, int epoll_fd, int* clients, int* num_clients)
{
	int sock_client = accept(sock, NULL, NULL);
	if (sock_client == -1)
	{
		return;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sock_client;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &ev);
	clients[*num_clients] = sock_client;
	*num_clients += 1;
	printf("Server accepted new connection\n-New connection: Client %d\n", sock_client);

	const char message[] = "[Server] Hello new client\n";
	if (send(sock_client, message, strlen(message), 0) == -1)
	{
		close(sock_client);
		return;
	}
}

void read_from_socket(int sock_client, int epoll_fd, int* clients, int num_clients)
{
	struct Message m;
	if (recv(sock_client, &m, sizeof(m), 0) == -1)
	{
		return;
	}

	char message[BUFFER_MESSAGE + BUFFER_NICKNAME] = { 0 };
	snprintf(message, sizeof(message), "[%s] %s", m.nickname, m.content);

	for (int i = 0; i < num_clients; i++)
	{
		if (clients[i] == sock_client)
			continue;

		if(send(clients[i], message, strlen(message), 0) == -1)
		{
			close(clients[i]);
			return;
		}
	}
}