#include "server.h"

int main(void)
{
	init();

	//SOCKET
	SOCKET sock = socket(ADDRESS_FAMILY, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		get_error();
	}

	SOCKADDR_IN socketAddress;
	socketAddress.sin_family = ADDRESS_FAMILY;
	socketAddress.sin_port = htons(LISTENING_PORT);
	socketAddress.sin_addr.s_addr = INADDR_ANY;

	//BIND
	if (bind(sock, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR)
	{
		closesocket(sock);
		get_error();
	}

	//LISTEN
	if (listen(sock, PENDING_QUEUE_MAXLENGTH) == SOCKET_ERROR)
	{
		closesocket(sock);
		get_error();
	}

	puts("Waiting for a client to connect...");

	//ACCEPT
	SOCKADDR_IN clientAddress = { 0 };
	int socketAdressLength = sizeof(clientAddress);
	SOCKET sock_client = accept(sock, (struct sockaddr*)&clientAddress, &socketAdressLength);
	if (sock_client == INVALID_SOCKET)
	{
		closesocket(sock_client);
		closesocket(sock);
		get_error();
	}
	
	printf("Client connected\n");

	int iResult;
	do {
		//READ
		char recvBuffer[BUFFER_SIZE] = { 0 };
		iResult = recv(sock_client, recvBuffer, BUFFER_SIZE, 0);
		if (iResult > 0)
		{
			printf("[CLIENT] %s", recvBuffer);
		}
		else if (iResult == 0)
		{
			printf("Connection closing...\n");
		}
		else
		{
			closesocket(sock_client);
			closesocket(sock);
			get_error();
		}

		//WRITE
		char sendBuffer[BUFFER_SIZE] = { 0 };
		fgets(sendBuffer, sizeof(sendBuffer), stdin);
		int sendBytes = send(sock_client, sendBuffer, (int)strlen(sendBuffer), 0);
		if (sendBytes == -1)
		{
			closesocket(sock_client);
			closesocket(sock);
			get_error();
		}

	} while (iResult > 0);

	closesocket(sock_client);
	closesocket(sock);
	close();

	return 0;
}

void get_error()
{
	fprintf(stderr, "Error: %d\n", WSAGetLastError());
	WSACleanup();
	exit(1);
}

void init()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		fprintf(stderr, "Error: %d\n", WSAGetLastError());
		exit(1);
	}
}

void close()
{
	WSACleanup();
}