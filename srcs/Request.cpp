/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:49:12 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/12 16:16:01 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include <iostream>


Request::Request(void)
{
}

Request::Request(std::string request) : _request(request)
{
	return;
}

Request::~Request(void)
{
	return;
}

Request::Request(Request const& src)
{
	this->_body = src._body;
	this->_httpVersion = src._httpVersion;
	this->_method = src._method;
	this->_type = src._type;
	this->_request = src._request;
	this->_url = src._url;
	this->_headers = src._headers;
}

Request& Request::operator=(Request const& src)
{
	if (this != &src)
	{
		this->_body = src._body;
		this->_httpVersion = src._httpVersion;
		this->_method = src._method;
		this->_type = src._type;
		this->_request = src._request;
		this->_url = src._url;
		this->_headers.clear();
		for (const auto& map_content : src._headers)
		{
			this->_headers[map_content.first] = map_content.second;
		}
	}	
	return *this;
}

void	Request::_getContentType(void)
{
	//Parses File type from url and set's the return content type to string attribute _type
	
	int	i = this->_url.find_last_of(".");
	std::string type = this->_url.substr(i + 1, this->_url.length());
	if (type == "css" || type == "js" || type == "html")
		this->_type = "text/" + type;
	else if (type == "jpg" || type == "png" || type == "jpeg" || type == "gif")
		this->_type = "image/" + type;
	else if (type == "mpeg" || type == "avi" || type == "mp4")
		this->_type = "video/" + type;
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
	
	this->_getContentType();
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