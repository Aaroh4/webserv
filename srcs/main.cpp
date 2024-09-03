/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:22:40 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/03 16:26:46 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include <iostream>

unsigned int convertip(std::string ip);
int	readconfig(std::string name);

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	readconfig(argv[1]);
	//std::cout << convertip("192.168.0.1") << std::endl;
}
