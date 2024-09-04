/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readconfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 16:18:51 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 12:39:08 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include <fstream>

void brackets(std::ifstream &configfile, std::string line, std::string type)
{
	if (line.find(type) != std::string::npos && line.find("{") != std::string::npos)
	{
		for (std::string line; std::getline(configfile, line) && line.find("}") == std::string::npos;)
		{
			std::cout << line << std::endl;
		}
	}
}

int	readconfig(std::string name)
{
	std::ifstream configfile(name);

	for (std::string line; std::getline(configfile, line);)
	{
		brackets(configfile, line, "server");
	}
	return (0);
}
