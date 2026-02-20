#include "../include/message.h"
#include <sys/socket.h>

size_t send_all(const int sock, const void *buffer, const size_t length)
{
    size_t total_sent = 0;
    const char *ptr = buffer;

    while (total_sent < length)
    {
        const ssize_t sent = send(sock, ptr + total_sent, length - total_sent, 0);

        if (sent <= 0)
        {
            return -1;
        }

        total_sent += sent;
    }

    return total_sent;
}

size_t recv_all(const int sock, void *buffer, const size_t length)
{
    size_t total_received = 0;
    char *ptr = buffer;

    while (total_received < length)
    {
        const ssize_t received = recv(sock, ptr + total_received, length - total_received, 0);

        if (received <= 0)
        {
            return -1;
        }

        total_received += received;
    }

    return total_received;
}