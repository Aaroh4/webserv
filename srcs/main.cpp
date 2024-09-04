/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:22:40 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 15:09:34 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <iostream>

int	readconfig(std::string name, ServerManager &manager);

int	main(int argc, char **argv)
{
	ServerManager	manager;

	if (argc != 2)
		return (1);
	readconfig(argv[1], manager);
	//std::cout << convertip("192.168.0.1") << std::endl;
	std::cout << manager.get_info(0).get_ip() << std::endl;
	std::cout << manager.get_info(0).get_port(0) << std::endl;
	std::cout << manager.get_info(1).get_ip() << std::endl;
}
