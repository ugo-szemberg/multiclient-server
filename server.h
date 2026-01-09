#ifndef __SERVER__
#define __ SERVER__

#include <sys/select.h>

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 5
#define BUFFER_SIZE 1024
#define ADDRESS_FAMILY AF_INET
#define TIMER_SELECT 3
#define MAX_EVENTS 64
#define MAX_CLIENTS 10

void accept_new_connection(int sock, int epoll_fd, int* clients, int* num_clients);
void read_from_socket(int sock_client, int epoll_fd, int* clients, int num_clients);

#endif