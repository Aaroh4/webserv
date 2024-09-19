/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readconfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 16:18:51 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/19 11:59:12 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <fstream>
#include <unordered_map>
#include <sstream>

ServerInfo	config_server(std::string temp, ServerInfo &server);
int brackets(std::string configfile, std::string type, ServerInfo &server);

std::string toLowerCase(const std::string str) 
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

int brackets(std::string configfile, std::string type, ServerInfo &server)
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
	if (type == "location")
	{
		server.setlocation(temp.substr(0, temp.find(" ")));
		//brackets(info, temp.substr(temp.find("{"), temp.size()), "location", server);
	}
	else if (type == "server")
	{
		config_server(temp, server);
		std::cout << "123\n";
	}
	return (temp.size());
}

ServerInfo	config_server(std::string temp, ServerInfo &server)
{
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
				 case 4:
				 	if (value.find("{") != std::string::npos)
				 		brackets(temp.substr(temp.find(value), temp.size()), "location", server);
				 	break;
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
	std::string		temp;
	int				i = 0;

	for (std::string line; std::getline(configfile, line);)
	{
		if (i > 0)
			temp += line + "\n";
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
			i++;
	}
	for (int info = 1; info <= i; info++)
	{
		ServerInfo server;
		temp = temp.substr(brackets(temp, "server", server), temp.size());
		manager.setnew_info(server);
		std::cout << server.getlocation() << std::endl;
	}
	//std::cout << manager.get_info()[0].getlocation() << std::endl;
	//std::cout << manager.get_info()[0].get_ip() << std::endl;
	return (0);
}
