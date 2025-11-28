#ifndef __SERVER__
#define __ SERVER__

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 5
#define BUFFER_SIZE 1024
#define ADDRESS_FAMILY AF_INET
#define TIMER_SELECT 3

void return_error(void);
void init(void);
void close(void);
void accept_new_connection(SOCKET sock, fd_set* sock_list, int* fd_max);
void read_from_socket(SOCKET sock_server, SOCKET sock_client, fd_set* sock_list, int fd_max);

#endif