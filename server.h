#ifndef __SERVER__
#define __SERVER__

#define LISTENING_PORT 5094
#define BUFFER_MESSAGE 1024
#define BUFFER_NICKNAME 30
#define MAX_CLIENTS 10
#define ADDRESS_FAMILY AF_INET
#define MAX_EVENTS 64

static void accept_new_connection(int sock, int epoll_fd, int* clients, int* num_clients);
static void read_from_socket(int sock_client, int epoll_fd, int* clients, int num_clients);
static void remove_client(int fd, int epoll_fd, int* clients, int* num_clients);

struct Message
{
    char nickname[BUFFER_NICKNAME];
    char content[BUFFER_MESSAGE];
};

static char record_messages[10000] = {0};

#endif