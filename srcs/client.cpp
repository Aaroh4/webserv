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
						 "\r\n";
	std::cout << "Headers length: " << request.length() << "\n";
	request += temp;
	std::cout << "request len?? " << request.length() << "\n";

	size_t total_sent = 0;
	std::string requesti = "POST / HTTP/1.1\r\n"
	"Transfer-Encoding: chunked\r\n"
	"\r\n"
	"4\r\n"
	"Wiki\r\n"
	"7\r\n"
	"pedia i\r\n"
	"B\r\n"
	"n \r\nchunks.\r\n"
	"0\r\n\r\n";
	while (total_sent < requesti.length()) {
    int bytes_sent = send(clientSocket, requesti.c_str() + total_sent, std::min(1024, static_cast<int>(requesti.length() - total_sent)), MSG_NOSIGNAL);
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