#include "../include/network.h"
#include "../include/message.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>

typedef struct
{
	int epoll_fd;
	int clients[MAX_CLIENTS];
	int num_clients;
	Message recorded_messages[100];
	int num_recorded_messages;
} server_context;

void send_recorded_messages(const int sock_client, const Message* recorded_messages, const int* num_recorded_messages)
{
	for (int i = 0; i < *num_recorded_messages; ++i)
	{
		send_all(sock_client, &recorded_messages[i], sizeof(Message));
	}
}

void accept_new_connection(server_context* context, const int sock)
{
	const int sock_client = accept(sock, NULL, NULL);
	if (sock_client == -1)
	{
		return;
	}

	if (context->num_clients >= MAX_CLIENTS)
	{
		close(sock_client);
		return;
	}

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sock_client;

	if (epoll_ctl(context->epoll_fd, EPOLL_CTL_ADD, sock_client, &event) == -1)
	{
		close(sock_client);
		return;
	}

	context->clients[context->num_clients] = sock_client;
	++context->num_clients;
	printf("Client connected : [ID %d]\n", sock_client);

	send_recorded_messages(sock_client, context->recorded_messages, &context->num_recorded_messages);
}

void remove_client(server_context* context, const int fd)
{
	epoll_ctl(context->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);

	for (int i = 0; i < context->num_clients; i++)
	{
		if (context->clients[i] == fd)
		{
			context->clients[i] = context->clients[context->num_clients - 1];
			--context->num_clients;
			break;
		}
	}
}

void read_from_socket(server_context* context, int sock_client)
{
	Message message = {0};
	if (recv_all(sock_client, &message, sizeof(message)) == -1)
	{
		printf("Client disconnected : [ID %d]\n", sock_client);
		remove_client(context, sock_client);
		return;
	}

	context->recorded_messages[context->num_recorded_messages] = message;
	++context->num_recorded_messages;

	for (int i = 0; i < context->num_clients; i++)
	{
		if (context->clients[i] != sock_client)
		{
			send_all(context->clients[i], &message, sizeof(message));
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

	server_context context = {0};

	context.epoll_fd = epoll_create1(0);
	if (context.epoll_fd == -1)
	{
		close(sock);
		return -1;
	}

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sock;
	if (epoll_ctl(context.epoll_fd, EPOLL_CTL_ADD, sock, &event) == -1)
	{
		close(sock);
		close(context.epoll_fd);
		return -1;
	}


	while (1)
	{
		struct epoll_event events[MAX_EVENTS];
		const int fds_count = epoll_wait(context.epoll_fd, events, MAX_EVENTS, -1);
		if (fds_count == -1)
		{
			break;
		}

		for (int i = 0; i < fds_count; ++i)
		{
			const int fd = events[i].data.fd;

			if (fd == sock)
			{
				accept_new_connection(&context, sock);
			}
			else
			{
				read_from_socket(&context, fd);
			}
		}
	}
	
	close(sock);
	close(context.epoll_fd);
	return 0;
}