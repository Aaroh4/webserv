/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:22:50 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/03 15:57:55 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <string>
#include <iostream>
#include <vector>

class ServerInfo
{
	public:
			ServerInfo();
			ServerInfo(const ServerInfo &);
			ServerInfo operator=(const ServerInfo &);
			~ServerInfo();
	private:
			unsigned int _ip;
			std::vector<int> _port;
};

#endif