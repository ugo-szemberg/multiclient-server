#ifndef __CLIENT__
#define __CLIENT__

#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_MESSAGE 1024
#define BUFFER_NICKNAME 30

int init(void);
void update(int sock);
void fill_nickname(char nickname[BUFFER_NICKNAME]);

struct Message
{
    char nickname[BUFFER_NICKNAME];
    char content[BUFFER_MESSAGE];
};

struct Message message;

#endif