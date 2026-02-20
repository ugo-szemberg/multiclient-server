#ifndef __CLIENT__
#define __CLIENT__

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_NICKNAME 30
#define BUFFER_CONTENT 1024
#define BUFFER_TIME 10
#define BUFFER_MESSAGE (BUFFER_NICKNAME + BUFFER_CONTENT + BUFFER_TIME)

static int connect_to_server(void);
static void client_loop(int sock);
static void set_nickname(char* nickname);
static void reset_message_content(void);

struct Message
{
    char nickname[BUFFER_NICKNAME];
    char content[BUFFER_CONTENT];
    char time[BUFFER_TIME];
};

static char nickname[BUFFER_NICKNAME];

#endif