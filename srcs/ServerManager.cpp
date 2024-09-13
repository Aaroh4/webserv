/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:23:09 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/13 11:13:23 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/Request.hpp"

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
	//int pollcount;

	for (size_t i = 0; i < this->get_info().size(); i++)
	{
		sockaddr_in serverAddress;
		int			opt = 1;

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(this->get_info()[i].get_port(0));
		serverAddress.sin_addr.s_addr = htonl(this->get_info()[i].get_ip());

		setsockopt(this->get_info()[i].getsocketfd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		bind(this->get_info()[i].getsocketfd(), (struct sockaddr *) &serverAddress, sizeof(serverAddress)); // ERROR CHECK HERE TOO LAZY
		fcntl(this->get_info()[i].getsocketfd(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		listen(this->get_info()[i].getsocketfd(), 5); // ERROR CHECK HERE TOO LAZY
	
		struct pollfd temp_s_pollfd;
		temp_s_pollfd.fd = this->get_info()[i].getsocketfd();
		temp_s_pollfd.events = POLLIN;
		poll_fds.push_back(temp_s_pollfd);
	}
 	while (true)
	{
		int pollcount = poll(poll_fds.data(), poll_fds.size(), 100);

		if (pollcount < 0)
			break ;
		for (size_t i = 0; i < poll_fds.size(); i++) 
		{
			// static int asd;
			// asd++;
			// std::cout << asd << ":" << i << std::endl;
			if (poll_fds[i].revents & POLLIN) 
			{
				if (i < this->get_info().size()) 
				{
					int client_socket = accept(poll_fds[i].fd, nullptr, nullptr);
					if (client_socket < 0) {
						perror("Accept failed");
						continue;
				}
					fcntl(client_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
					struct pollfd client_pollfd;
					client_pollfd.fd = client_socket;
					client_pollfd.events = POLLIN;
					poll_fds.push_back(client_pollfd);
					std::cout << "New client connected on server " << i << std::endl;
				} 
				else 
				{
					char buffer[1024] = {0};
					int client_socket = poll_fds[i].fd;
					int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
					
					if (bytes_received <= 0) 
					{
						std::cout << "Client disconnected" << std::endl;
						close(client_socket);
						poll_fds.erase(poll_fds.begin() + i);
						i--;
					}
					else 
					{
						//std::cout << buffer << std::endl;
						Request request(buffer);
						request.parse();
						Response respond(request);
						respond.respond(client_socket);
					}
				}
			}
		}
	}
}


void ServerManager::setnew_info(ServerInfo server)
{
	this->_info.push_back(server);
}

std::vector<ServerInfo> ServerManager::get_info()
{
	return (this->_info);
}