#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>

int main()
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddress;
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	if (inet_pton(AF_INET, "172.50.70.131", &serverAddress.sin_addr) == 0)
		return -1;

	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	const char* request = "GET local_host:8080 HTTP/1.1";
	
	send(clientSocket, request, strlen(request), 0);

	close(clientSocket);
	return (0);
}