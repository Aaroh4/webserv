/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readconfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 16:18:51 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/03 16:52:37 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include <fstream>

int	readconfig(std::string name)
{
	std::ifstream configfile(name);
	bool	foundserver = false;

	for (std::string line; std::getline(configfile, line);)
	{
		if (line.find("server") && foundserver == false)
			foundserver = true;
		else if (line.find("server") && foundserver != false)
			return (1);
		if (line.find("{") && foundserver == true)
		{
			std::cout << line << std::endl;
			for (std::string line; line.find("}"); std::getline(configfile, line))
			{
				std::cout << line << std::endl;
			}
		}
	}
	return (0);
}
