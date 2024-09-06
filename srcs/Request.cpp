/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:49:12 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/06 15:22:59 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(std::string request) : _request(request)
{
	return;
}

Request::Request(void)
{
	return;
}

Request::parseMethod(void)
{
	const char* methods[3] = {"GET", "POST", "DELETE"}
	int i = this->_request.find_first_of(" ");
	std::string temp = this->_request.substr(0, i);
	int index = 0;
	
	for (index; index < 3; index++)
	{
		if (temp == methods[i])
		{
			this->_method = temp;
			break;
		}
	}
	if (index == 3)
		//ERROR
}

Request::parseUrl(void)
{
	
}