#include "client.h"

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
	printf("---CLIENT---\n\n");

	int sock = init();

	logic(sock);

	close(sock);

	return 0;
}

int init(void)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		fprintf(stderr, "Error:\n");
		exit(1);
	}

	struct sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(LISTENING_PORT);

	int inetReturnCode = inet_pton(AF_INET, CONNECTION_HOST, &socketAddress.sin_addr);
	if (inetReturnCode != 1)
	{
		fprintf(stderr, "Error:\n");
		exit(1);
	}

	int socketAddressLength = sizeof(socketAddress);
	int connectionStatus = connect(sock, (struct sockaddr*)&socketAddress, socketAddressLength);
	if (connectionStatus == -1)
	{
		fprintf(stderr, "Error:\n");
		exit(1);
	}
	else
	{
		printf("Connected to the server.\n");
	}

	return sock;
}

void logic(int sock)
{
	int iResult;
	do {
		char recvBuffer[BUFFER_SIZE] = { 0 };
		iResult = (int)recv(sock, recvBuffer, BUFFER_SIZE, 0);
		if (iResult > 0)
		{
			printf("%s", recvBuffer);
		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("recv failed:\n");
		}

		char sendBuffer[BUFFER_SIZE] = { 0 };
		fgets(sendBuffer, sizeof(sendBuffer), stdin);

		int sendBytes = (int)send(sock, sendBuffer, (int)strlen(sendBuffer), 0);
		if (sendBytes == -1)
		{
			fprintf(stderr, "Error:\n");
			exit(1);
		}
	} while (iResult > 0);
}