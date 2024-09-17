/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readconfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 16:18:51 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/10 14:32:07 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <fstream>
#include <unordered_map>
#include <sstream>

std::string toLowerCase(const std::string str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

ServerInfo	config_server(std::string temp)
{
	ServerInfo server;
	std::vector<std::string> string_to_case
	{
		{"host: "},
		{"host:"},
		{"port: "},
		{"port:"},
		{"location "},
	};

	server.setsocketfd(socket(AF_INET, SOCK_STREAM, 0));
	for (size_t j = 0; j < string_to_case.size(); j++)
	{
		size_t pos = toLowerCase(temp).find(string_to_case.at(j));
		if (pos != std::string::npos)
		{
			std::string	value;
			std::istringstream stream(temp.substr(pos + string_to_case.at(j).size(), std::string::npos));
			std::getline(stream, value, '\n');
			switch (j)
			{
				case 0:
				case 1:
					server.set_ip(value);
					break;
				case 2:
				case 3:
					server.setnew_port(std::stoi(value));
					break;
				default:
					break;
			}
		}
	}
	//std::cout << server.get_ip() << std::endl;
		//std::cout << line.substr(7, std::string::npos) << std::endl;
	std::cout << "asd\n";
	return (server);
}

void brackets(std::ifstream &configfile, std::string line, std::string type, ServerManager &manager)
{
	std::string temp;
	
	if (line.find(type) != std::string::npos && line.find("{") != std::string::npos)
	{
		for (std::string line; std::getline(configfile, line) && line.find("}") == std::string::npos;)
			temp += line + "\n";
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
