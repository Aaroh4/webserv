/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:46:27 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/04 14:28:24 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerInfo.hpp"
#include <string>
#include <iostream>
#include <vector>

class ServerManager
{
	public:
			ServerManager();
			ServerManager(const ServerManager &);
			ServerManager operator=(const ServerManager &);
			~ServerManager();

			void	setnew_info(ServerInfo server);
			ServerInfo get_info(int num);
	private:
			std::vector<ServerInfo> _info;
};

#endif