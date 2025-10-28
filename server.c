#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define LISTENING_PORT 5094
#define PENDING_QUEUE_MAXLENGTH 1
#define BUFFER_SIZE 1024
#define ADDRESS_FAMILY AF_INET

int main(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	SOCKET socketFD = socket(ADDRESS_FAMILY, SOCK_STREAM, 0);
	if (socketFD == INVALID_SOCKET)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	struct sockaddr_in socketAddress;
	socketAddress.sin_family = ADDRESS_FAMILY;
	socketAddress.sin_port = htons(LISTENING_PORT);
	socketAddress.sin_addr.s_addr = INADDR_ANY;

	int bindReturnCode = bind(socketFD, (struct sockaddr*)&socketAddress, sizeof(socketAddress));

	if (bindReturnCode == SOCKET_ERROR)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	if (listen(socketFD, PENDING_QUEUE_MAXLENGTH) == -1)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	puts("Waiting for a client to connect...");

	int socketAdressLength = sizeof(socketAddress);
	SOCKET connectedSocketFD = accept(socketFD, (struct sockaddr*)&socketAddress, &socketAdressLength);
	if (connectedSocketFD == -1)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}
	else
	{
		printf("Client connected\n");
	}

	int iResult;
	do {
		char recvBuffer[BUFFER_SIZE] = { 0 };
		iResult = recv(connectedSocketFD, recvBuffer, BUFFER_SIZE, 0);
		if (iResult > 0)
		{
			printf("%s", recvBuffer);
		}
		else if (iResult == 0)
		{
			printf("Connection closing...\n");
		}
		else
		{
			fprintf(stderr, "Error: %d\n", WSAGetLastError());
			closesocket(connectedSocketFD);
			WSACleanup();
			exit(1);
		}

		char sendBuffer[BUFFER_SIZE] = { 0 };
		fgets(sendBuffer, sizeof(sendBuffer), stdin);
		int sendBytes = send(connectedSocketFD, sendBuffer, (int)strlen(sendBuffer), 0);
		if (sendBytes == -1)
		{
			fprintf(stderr, "Error: %d\n", WSAGetLastError());
			exit(1);
		}

	} while (iResult > 0);

	closesocket(connectedSocketFD);
	closesocket(socketFD);
	WSACleanup();

	return 0;
}