/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:49:12 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/10 10:27:25 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(std::string request) : _request(request)
{
	return;
}

Request::~Request(void)
{
	return;
}

//Parses method and put's it to string attribute _method, then erases it from the request
void	Request::parseMethod(void)
{
	const char* methods[3] = {"GET", "POST", "DELETE"};
	int i = this->_request.find_first_of(" ");
	this->_method = this->_request.substr(0, i);
	int index = 0;
	
	for (index; index < 3; index++)
	{
		if (this->_method == methods[i])
			break;
	}
	if (index == 3)
		/*ERROR*/;
	this->_request.erase(0, this->_method.length() + 1);
}

//Parses URI and put's it to string attribute _url, then erases it from the request
void	Request::parseUrl(void)
{
	int i = this->_request.find_first_of(" ");

	this->_url = this->_request.substr(0, i);
	this->_request.erase(0, this->_url.length() + 1);
}

//Parses headers line by line and adds the header(before :) and value (after :) to map container attribute _headers
void Request::parseHeaders(void)
{
	std::string line;

	size_t	lineEnd = 0;
	size_t	i = 0;

	while (line != "\r\n")
	{
		lineEnd = this->_request.find_first_of("\n");
		line = this->_request.substr(0, lineEnd);
		i = line.find_first_of(":");
		if (i == std::string::npos)
		{
			i = this->_request.find_last_of("\n");
			this->_request.erase(0, i + 1);
			break;
		}
		this->_headers[line.substr(0, i)] = line.substr(i + 2, lineEnd);
		this->_request.erase(0, lineEnd + 1);
	}
	this->_body = this->_request;
}