/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:23:09 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 14:39:02 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerManager.hpp"

ServerManager::ServerManager()
{}

ServerManager::ServerManager(const ServerManager &input)
{
	(void)input; // NEEDS TO BE CHANGED IM TOO LAZY RIGHT NOW
}

ServerManager ServerManager::operator=(const ServerManager &input)
{
	(void)input; // NEEDS TO BE CHANGED IM TOO LAZY RIGHT NOW
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