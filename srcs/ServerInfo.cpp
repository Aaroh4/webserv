/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:02:43 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 14:42:06 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"

ServerInfo::ServerInfo()
{}

ServerInfo::ServerInfo(const ServerInfo &input)
{
	this->_ip = input._ip;
	this->_port = input._port;
}

ServerInfo ServerInfo::operator=(const ServerInfo &input)
{
	if (this != &input)
	{
		this->_ip = input._ip;
		this->_port = input._port;
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
