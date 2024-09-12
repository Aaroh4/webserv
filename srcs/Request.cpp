/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:49:12 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/12 10:24:50 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include <iostream>

Request::Request(std::string request) : _request(request)
{
	return;
}

Request::~Request(void)
{
	return;
}

void	Request::_parseRequestLine(void)
{
	//Parses method and put's it to string attribute _method, then erases it from the request
	
	const char* methods[3] = {"GET", "POST", "DELETE"};
	int i = this->_request.find_first_of(" ");
	this->_method = this->_request.substr(0, i);
	int index;
	
	for (index = 0; index < 3; index++)
	{
		if (this->_method == methods[i])
			break;
	}
	if (index == 3)
		/*ERROR*/;
	this->_request.erase(0, this->_method.length() + 1);

	//Parses URI and put's it to string attribute _url, then erases it from the request
	
	i = this->_request.find_first_of(" ");

	this->_url = this->_request.substr(0, i);
	this->_request.erase(0, this->_url.length() + 1);
	
	//Parses HTTP Veresion nd put's it to string attribute _httpVersion, then erases it from the request

	i = this->_request.find_first_of("\n");
	this->_httpVersion = this->_request.substr(0, i);
	this->_request.erase(0, this->_httpVersion.length() + 1);
}

//Parses headers line by line and adds the header(before :) and value (after :) to map container attribute _headers
void	Request::_parseHeaders(void)
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

void	Request::parse(void)
{
	this->_parseRequestLine();
	this->_parseHeaders();
}

std::string  Request::getMethod(void) const
{
	return this->_method;
}

std::string  Request::getUrl(void) const
{
	return this->_url;
}

std::string  Request::getBody(void) const
{
	return this->_body;
}

std::string  Request::getHttpVersion(void) const
{
	return this->_httpVersion;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return this->_headers;
}