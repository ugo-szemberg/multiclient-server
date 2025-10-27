#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

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

	char buffer[BUFFER_SIZE] = { 0 };
	int receivedBytes = recv(connectedSocketFD, buffer, BUFFER_SIZE, 0);
	if (receivedBytes == SOCKET_ERROR)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	printf("Client: %s\n", buffer);
	const char message[] = "Hello client, I'm the server.";
	int sendBytes = send(connectedSocketFD, message, (int)strlen(message), 0);
	if (sendBytes == -1)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	closesocket(connectedSocketFD);
	closesocket(socketFD);
	WSACleanup();

	system("pause");
	return 0;
}