#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fstream>
#include <algorithm>


int main()
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in serverAddress;
	std::ifstream infile("big.txt");
	std::string buffer;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) == 0)
		return -1;
	std::string temp;
	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	while (std::getline(infile, buffer))
	{
		temp += buffer;
	}
	size_t len = temp.length();
	std::cout << "len?? " << len << "\n";
	std::string request = "POST / HTTP/1.1\r\n"
						 "Content-Length: 6360209\r\n"
						 "\r\n\r\n";
	std::cout << "Headers length: " << request.length() << "\n";
	request += temp;
	std::cout << "request len?? " << request.length() << "\n";
	//int bytes_sent = send(clientSocket, request.c_str(), request.length(), 0);

	//std::cout << "bytes sent: " << bytes_sent << "\n";
	size_t total_sent = 0;
	while (total_sent < request.length()) {
    int bytes_sent = send(clientSocket, request.c_str() + total_sent, std::min(1024, static_cast<int>(request.length() - total_sent)), MSG_NOSIGNAL);
    if (bytes_sent <= 0) {
        std::cerr << "Send error or connection closed by server" << std::endl;
        break;
    }
    total_sent += bytes_sent;
    usleep(1000);  // brief delay
}
	close(clientSocket);
	return (0);
}