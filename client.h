#ifndef __CLIENT__
#define __CLIENT__

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_SIZE 1024

SOCKET init(void);
void play_chat(SOCKET sock);
void close(SOCKET sock);

#endif