#ifndef __CLIENT__
#define __CLIENT__

#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_SIZE 1024

int init(void);
void logic(int sock);

#endif