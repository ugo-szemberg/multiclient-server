#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_SIZE 1024

void Initialization(SOCKET* connectedSocket);
void Chat(SOCKET* connectedSocket);
void CleanUp(SOCKET* connectedSocket);

int main(void)
{
	SOCKET connectedSocket;

	Initialization(&connectedSocket);
	
	Chat(&connectedSocket);

	CleanUp(&connectedSocket);

	return 0;
}

void Initialization(SOCKET* connectedSocket)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	*connectedSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (*connectedSocket == INVALID_SOCKET)
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
	int connectionStatus = connect(*connectedSocket, (struct sockaddr*)&socketAddress, socketAddressLength);
	if (connectionStatus == -1)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}
	else
	{
		printf("Connected to the server.\n");
	}
}

void Chat(SOCKET* connectedSocket)
{
	int iResult;
	do {
		char sendBuffer[BUFFER_SIZE] = { 0 };
		fgets(sendBuffer, sizeof(sendBuffer), stdin);

		int sendBytes = send(*connectedSocket, sendBuffer, (int)strlen(sendBuffer), 0);
		if (sendBytes == SOCKET_ERROR)
		{
			fprintf(stderr, "Error: %d\n", WSAGetLastError());
			exit(1);
		}

		char recvBuffer[BUFFER_SIZE] = { 0 };
		iResult = recv(*connectedSocket, recvBuffer, BUFFER_SIZE, 0);
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
			printf("recv failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);
}

void CleanUp(SOCKET* connectedSocket)
{
	closesocket(*connectedSocket);
	WSACleanup();
}