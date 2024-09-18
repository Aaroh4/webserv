/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:22:40 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/18 15:51:07 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int	readconfig(std::string name, ServerManager &manager);

//void	test_connect(ServerManager manager)
//{
//	int	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//	sockaddr_in serverAddress;

//	serverAddress.sin_family = AF_INET;
//	serverAddress.sin_port = htons(8080);
//	serverAddress.sin_addr.s_addr = htonl(manager.get_info(0).get_ip());

//	bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
//	listen(serverSocket, 5);
//	int clientSocket = accept(serverSocket, nullptr, nullptr);
//	char buffer[1024] = {0};
//	recv(clientSocket, buffer, sizeof(buffer), 0);
//	std::cout << "Message from client: " << buffer << std::endl;
//    close(serverSocket);
//}

int	main(int argc, char **argv)
{
	ServerManager	manager;

	if (argc != 2)
		return (1);
	readconfig(argv[1], manager);
	//std::cout << convertip("192.168.0.1") << std::endl;
	//manager.get_info()[0].setlocation("124");
	//manager.get_info()[0].printlocation();
	//std::cout << manager.get_info()[0].get_port(0) << std::endl;
	//std::cout << manager.get_info()[1].get_ip() << std::endl;
	//std::cout << manager.get_info()[1].get_port(0) << std::endl;
	//std::cout << manager.get_info(1).get_ip() << std::endl;
	//test_connect(manager);
	manager.start_servers();
}
