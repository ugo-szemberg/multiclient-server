#ifndef __SERVER__
#define __ SERVER__

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 1
#define BUFFER_SIZE 1024
#define ADDRESS_FAMILY AF_INET

void get_error();
void init();
void close();

#endif