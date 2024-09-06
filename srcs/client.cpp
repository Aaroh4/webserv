/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 13:04:20 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/06 13:42:28 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	serverAddress.sin_port = htons(8000);
	if (inet_pton(AF_INET, "192.168.0.100", &serverAddress.sin_addr) == 0)
		return -1;
	
	printf("hei\n");
	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	printf("moii\n");
	const char* request = "GET local_host:8080 HTTP/1.1";
	
	send(clientSocket, request, strlen(request), 0);

	close(clientSocket);
	return (0);
}