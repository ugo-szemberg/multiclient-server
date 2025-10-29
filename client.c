#include "client.h"

int main(void)
{
	printf("---CLIENT---\n\n");

	SOCKET sock = init();

	play_chat(sock);

	close(sock);

	return 0;
}

SOCKET init(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	struct sockaddr_in socketAddress;
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(LISTENING_PORT);

	int inetReturnCode = inet_pton(AF_INET, CONNECTION_HOST, &socketAddress.sin_addr);
	if (inetReturnCode != 1)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	int socketAddressLength = sizeof(socketAddress);
	int connectionStatus = connect(sock, (struct sockaddr*)&socketAddress, socketAddressLength);
	if (connectionStatus == SOCKET_ERROR)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}
	else
	{
		printf("Connected to the server.\n");
	}

	return sock;
}

void play_chat(SOCKET sock)
{
	int iResult;
	do {
		char sendBuffer[BUFFER_SIZE] = { 0 };
		fgets(sendBuffer, sizeof(sendBuffer), stdin);

		int sendBytes = send(sock, sendBuffer, (int)strlen(sendBuffer), 0);
		if (sendBytes == SOCKET_ERROR)
		{
			fprintf(stderr, "Error: %d\n", WSAGetLastError());
			exit(1);
		}

		char recvBuffer[BUFFER_SIZE] = { 0 };
		iResult = recv(sock, recvBuffer, BUFFER_SIZE, 0);
		if (iResult > 0)
		{
			printf("[SERVER] %s", recvBuffer);
		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("recv failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);
}

void close(SOCKET sock)
{
	closesocket(sock);
	WSACleanup();
}