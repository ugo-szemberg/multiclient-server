#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define CONNECTION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_SIZE 1024

int main(void)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	SOCKET socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFD == INVALID_SOCKET)
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
	int connectionStatus = connect(socketFD, (struct sockaddr*)&socketAddress, socketAddressLength);
	if (connectionStatus == -1)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	const char message[] = "Hello server, I'm the client.";
	int sendBytes = send(socketFD, message, (int)strlen(message), 0);
	if (sendBytes == SOCKET_ERROR)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}

	char buffer[BUFFER_SIZE] = { 0 };
	int receivedBytes = recv(socketFD, buffer, BUFFER_SIZE, 0);
	if (receivedBytes == SOCKET_ERROR)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}
	printf("Server: %s\n", buffer);

	closesocket(socketFD);
	WSACleanup();

	system("pause");
	return 0;
}