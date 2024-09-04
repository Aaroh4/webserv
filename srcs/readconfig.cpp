/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readconfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 16:18:51 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 15:09:07 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <fstream>

ServerInfo	config_server(std::vector<std::string> temp)
{
	ServerInfo server;
		
	for (size_t i = 0; i < temp.size() ; i++)
	{
		if (temp[i].find("host: ") != std::string::npos)
			server.set_ip(temp[i].substr(7, std::string::npos));
		if (temp[i].find("port: ") != std::string::npos)
			server.setnew_port(stoi(temp[i].substr(7, std::string::npos)));
	}
	//std::cout << server.get_ip() << std::endl;
		//std::cout << line.substr(7, std::string::npos) << std::endl;
	return (server);
}

void brackets(std::ifstream &configfile, std::string line, std::string type, ServerManager &manager)
{
	std::vector<std::string> temp;
	
	if (line.find(type) != std::string::npos && line.find("{") != std::string::npos)
	{
		for (std::string line; std::getline(configfile, line) && line.find("}") == std::string::npos;)
			temp.push_back(line);
		if (type == "server")
			manager.setnew_info(config_server(temp));
		temp.clear();
	}
}

int	readconfig(std::string name, ServerManager &manager)
{
	std::ifstream configfile(name);

	for (std::string line; std::getline(configfile, line);)
	{
		brackets(configfile, line, "server", manager);
	}
	return (0);
}
