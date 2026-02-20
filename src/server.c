#include "../include/server.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>

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

void accept_new_connection(const int sock, const int epoll_fd, int* clients, int* num_clients)
{
	const int sock_client = accept(sock, NULL, NULL);
	if (sock_client == -1)
	{
		return;
	}

	if (*num_clients >= MAX_CLIENTS)
	{
		close(sock_client);
		return;
	}

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sock_client;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &event) == -1)
	{
		close(sock_client);
		return;
	}

	clients[*num_clients] = sock_client;
	++(*num_clients);
	printf("New client connected : Client %d\n", sock_client);

	//if (strlen(record_messages) > 0)
	//{
	//	uint32_t size = htonl(strlen(record_messages));
	//	send_all(sock_client, &size, sizeof(size));
	//	send_all(sock_client, record_messages, strlen(record_messages));
	//}
}

void read_from_socket(const int sock_client, const int epoll_fd, int* clients, int* num_clients)
{
	struct Message message = {0};
	if (recv_all(sock_client, &message, sizeof(message)) == -1)
	{
		printf("Client disconnected : Client %d\n", sock_client);
		remove_client(sock_client, epoll_fd, clients, num_clients);
		return;
	}

	//char send_message[BUFFER_MESSAGE + 4];
	//snprintf(send_message, sizeof(send_message), "[%s] %s %s", message.nickname, message.content, message.time);

	//strcat(record_messages, send_message);

	for (int i = 0; i < *num_clients; i++)
	{
		if (clients[i] != sock_client)
		{
			send_all(clients[i], &message, sizeof(message));
		}
	}
}

void remove_client(const int fd, const int epoll_fd, int* clients, int* num_clients)
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);

	for (int i = 0; i < *num_clients; i++)
	{
		if (clients[i] == fd)
		{
			clients[i] = clients[*num_clients - 1];
			--(*num_clients);
			break;
		}
	}
}

int main(void)
{
	printf("---SERVER---\n\n");

	const int sock = socket(ADDRESS_FAMILY, SOCK_STREAM, 0);
	if (sock == -1)
	{
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

	if (listen(sock, MAX_CLIENTS) == -1)
	{
		close(sock);
		return -1;
	}

	const int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		close(sock);
		return -1;
	}

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &event) == -1)
	{
		close(sock);
		close(epoll_fd);
		return -1;
	}

	int clients[MAX_CLIENTS];
	int num_clients = 0;

	while (1)
	{
		struct epoll_event events[MAX_EVENTS];
		const int fds_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (fds_count == -1)
		{
			break;
		}

		for (int i = 0; i < fds_count; ++i)
		{
			const int fd = events[i].data.fd;

			if (fd == sock)
			{
				accept_new_connection(sock, epoll_fd, clients, &num_clients);
			}
			else
			{
				read_from_socket(fd, epoll_fd, clients, &num_clients);
			}
		}
	}
	
	close(sock);
	close(epoll_fd);
	return 0;
}