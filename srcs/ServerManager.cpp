/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:23:09 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 15:03:37 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerManager.hpp"

ServerManager::ServerManager()
{}

ServerManager::ServerManager(const ServerManager &input)
{
	this->_info = input._info;
}

ServerManager ServerManager::operator=(const ServerManager &input)
{
	if (this != &input)
	{
		this->_info = input._info;
	}
	return (input);
}

ServerManager::~ServerManager()
{
}

void	ServerManager::start_servers()
{
	std::vector<struct pollfd> poll_fds;

	for (size_t i = 0; i < this->get_info().size(); i++)
	{
		std::cout << "hello\n";
		sockaddr_in serverAddress;

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(this->get_info()[i].get_port(0));
		serverAddress.sin_addr.s_addr = htonl(this->get_info()[i].get_ip());

		bind(this->get_info()[i].getsocketfd(), (struct sockaddr *) &serverAddress, sizeof(serverAddress)); // ERROR CHECK HERE TOO LAZY
		listen(this->get_info()[i].getsocketfd(), 5); // ERROR CHECK HERE TOO LAZY
	
		struct pollfd server_pollfd;
		server_pollfd.fd = this->get_info()[i].getsocketfd();
		server_pollfd.events = POLLIN | POLLOUT;
		poll_fds.push_back(server_pollfd);
	}
	while (true)
	{
		int pollcount = poll(poll_fds.data(), poll_fds.size(), -1);

		if (pollcount < 0)
			break ;
	 for (size_t i = 0; i < poll_fds.size(); i++) {
            if (poll_fds[i].revents & POLLIN) {
                // If it's a server socket, accept the connection
                if (i < this->get_info().size()) {
                    int client_socket = accept(poll_fds[i].fd, nullptr, nullptr);
                    if (client_socket < 0) {
                        perror("Accept failed");
                        continue;
                    }

                    // Add new client socket to poll_fds
                    struct pollfd client_pollfd;
                    client_pollfd.fd = client_socket;
                    client_pollfd.events = POLLIN; // Monitor for incoming data from client
                    poll_fds.push_back(client_pollfd);

                    std::cout << "New client connected on server " << i << std::endl;
                } 
                // If it's a client socket, handle incoming data
                else {
                    char buffer[1024] = {0};
                    int client_socket = poll_fds[i].fd;
                    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                    
                    if (bytes_received <= 0) {
                        // Client disconnected, close socket
                        std::cout << "Client disconnected" << std::endl;
                        close(client_socket);
                        poll_fds.erase(poll_fds.begin() + i);
                        i--; // Adjust index after removal
                    } else {
                        std::cout << "Message from client: " << buffer << std::endl;
                    }
                }
            }
		}
	}
	//close(this->get_info()[i].getsocketfd());
}

void ServerManager::setnew_info(ServerInfo server)
{
	this->_info.push_back(server);
}

std::vector<ServerInfo> ServerManager::get_info()
{
	return (this->_info);
}
