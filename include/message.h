#ifndef TOOLS_H
#define TOOLS_H

#include <unistd.h>

#define BUFFER_NICKNAME 30
#define BUFFER_CONTENT 1024
#define BUFFER_TIME 10

struct Message
{
    char nickname[BUFFER_NICKNAME];
    char content[BUFFER_CONTENT];
    char time[BUFFER_TIME];
};

size_t send_all(int sock, const void *buffer, size_t length);
size_t recv_all(int sock, void *buffer, size_t length);

#endif