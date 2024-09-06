/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conversion.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:22:36 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/03 15:35:12 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"

unsigned int convertip(std::string ip)
{
	unsigned int returnip[4];
	
	int i = 0;
	while (i < 4)
	{	
		returnip[i] = stoi(ip.substr(0, ip.find('.')));
		ip.erase(0, ip.find('.') + 1);
		i++;
	}
	return ((returnip[0] << 24) | (returnip[1] << 16) | (returnip[2] << 16) | (returnip[3]));
}
