#ifndef __SERVER__
#define __SERVER__

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 5
#define BUFFER_MESSAGE 1024
#define BUFFER_NICKNAME 30
#define ADDRESS_FAMILY AF_INET
#define TIMER_SELECT 3
#define MAX_EVENTS 64
#define MAX_CLIENTS 10

void accept_new_connection(int sock, int epoll_fd, int* clients, int* num_clients);
void read_from_socket(int sock_client, int epoll_fd, int* clients, int num_clients);

struct Message
{
    char nickname[BUFFER_NICKNAME];
    char content[BUFFER_MESSAGE];
};

#endif