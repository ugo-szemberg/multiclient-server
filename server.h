#ifndef __SERVER__
#define __ SERVER__

#include <sys/select.h>

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 5
#define BUFFER_SIZE 1024
#define ADDRESS_FAMILY AF_INET
#define TIMER_SELECT 3

void accept_new_connection(int sock, fd_set* sock_list, int* fd_max);
void read_from_socket(int sock_server, int sock_client, fd_set* sock_list, int fd_max);

#endif