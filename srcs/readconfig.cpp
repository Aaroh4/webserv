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

ServerInfo	config_server(std::string temp, ServerManager &manager);

std::string toLowerCase(const std::string str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

void brackets(std::string configfile, std::string type, ServerManager &manager)
{
	std::string 		temp;
	int					brackets = 1;
	std::istringstream	file(configfile);

	for (std::string line; std::getline(file, line) && brackets != 0;)
	{
		temp += line + "\n";
		if (line.find("{") != std::string::npos)
			brackets++;
		if (line.find("}") != std::string::npos && brackets > 0)
			brackets--;
	}
	std::cout << temp << std::endl;
	if (type == "server")
		manager.setnew_info(config_server(temp, manager));
	else if (type == "location")
		std::cout << "hello\n";
}

ServerInfo	config_server(std::string temp, ServerManager &manager)
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

	(void) manager;
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
				// case 4:
				// 	brackets(temp, "location", manager);
				// 	break;
				default:
					break;
			}
		}
	}
	return (server);
}

int	readconfig(std::string name, ServerManager &manager)
{
	std::ifstream	configfile(name);
	std::string		filesave;

	for (std::string line; std::getline(configfile, line);)
		filesave += line + "\n";
	for (std::string line; std::getline(std::istringstream(filesave), line);)
	{
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
		{
			filesave = filesave.substr(filesave.find("{") + 1, filesave.size());
			brackets(filesave, "server", manager);
			while (1)
			{}
		}
	}
	return (0);
}
