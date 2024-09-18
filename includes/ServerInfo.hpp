/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:22:50 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/18 16:15:27 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include "libraries.hpp"

class ServerInfo
{
	public:
			ServerInfo();
			ServerInfo(const ServerInfo &);
			ServerInfo operator=(const ServerInfo &);
			~ServerInfo();

			void				set_ip(std::string ip);
			unsigned int		get_ip() const;

			void				setnew_port(int port);
			int					get_port(int num) const;

			void				setsocketfd(int fd);
			int					getsocketfd();

			void				setlocation(std::string name);
			std::string			getlocation();

	private:
			unsigned int		_ip;
			std::vector<int>	_port;
			int					_socketfd;
			std::string 		locations;
			std::vector <struct pollfd> _pollfds;
};

unsigned int convertip(std::string ip);

#endif