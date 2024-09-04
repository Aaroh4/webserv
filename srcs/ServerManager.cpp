/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:23:09 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 15:03:37 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerManager.hpp"

ServerManager::ServerManager()
{}

ServerManager::ServerManager(const ServerManager &input)
{
	this->_info = input._info;
}

ServerManager ServerManager::operator=(const ServerManager &input)
{
	if (this != &input)
	{
		this->_info = input._info;
	}
	return (input);
}

ServerManager::~ServerManager()
{
}

void ServerManager::setnew_info(ServerInfo server)
{
	this->_info.push_back(server);
}

ServerInfo ServerManager::get_info(int num)
{
	return (this->_info[num]);
}
