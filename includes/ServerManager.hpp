/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:46:27 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/16 13:06:16 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerInfo.hpp"
#include "libraries.hpp"
#include <algorithm> 

class ServerManager
{
	public:
			ServerManager();
			ServerManager(const ServerManager &);
			ServerManager operator=(const ServerManager &);
			~ServerManager();

			void	start_servers();
			void	setnew_info(ServerInfo server);
			std::vector<ServerInfo> get_info();
	private:
			std::vector<ServerInfo> _info;
};

#endif