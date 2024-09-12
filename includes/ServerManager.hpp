/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:46:27 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/12 12:54:43 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerInfo.hpp"
#include "libraries.hpp"

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