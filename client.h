#ifndef __CLIENT__
#define __CLIENT__

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_MESSAGE 1024
#define BUFFER_NICKNAME 30

static int connect_to_server(void);
static void client_loop(int sock);
static void set_nickname(const char* nickname);

struct Message
{
    char nickname[BUFFER_NICKNAME];
    char content[BUFFER_MESSAGE];
};

struct Message message;

#endif