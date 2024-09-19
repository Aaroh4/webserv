/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:02:43 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/19 15:16:13 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"

ServerInfo::ServerInfo()
{}

ServerInfo::ServerInfo(const ServerInfo &input)
{
	this->_ip = input._ip;
	this->_port = input._port;
	this->_socketfd = input._socketfd;
	this->_locations = input._locations;
	this->_locationinfo = input._locationinfo;
}

ServerInfo ServerInfo::operator=(const ServerInfo &input)
{
	if (this != &input)
	{
		this->_ip = input._ip;
		this->_port = input._port;
		this->_socketfd = input._socketfd;
		this->_locations = input._locations;
		this->_locationinfo = input._locationinfo;
	}
	return (input);
}

ServerInfo::~ServerInfo()
{
}

void ServerInfo::set_ip(std::string ip)
{
	this->_ip = convertip(ip);
}
unsigned int		ServerInfo::get_ip() const
{
	return (this->_ip);
}

void	ServerInfo::setnew_port(int port)
{
	this->_port.push_back(port);
}
			
			
int		ServerInfo::get_port(int num) const
{
	return(this->_port[num]);
}

void	ServerInfo::setsocketfd(int fd)
{
	this->_socketfd = fd;
}

int		ServerInfo::getsocketfd()
{
	return (this->_socketfd);
}

void	ServerInfo::setnewlocation(location input)
{
	this->_locationinfo[input.name] = input;
}

std::unordered_map<std::string, location>	ServerInfo::getlocationinfo() const
{
	return (this->_locationinfo);
}
