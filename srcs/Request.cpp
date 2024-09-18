/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:49:12 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/18 13:46:42 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include <iostream>
#include <climits>

Request::Request(void) : _sanitizeStatus(0)
{
}

Request::Request(std::string request) :  _sanitizeStatus(0), _request(request) 
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
	
	size_t	i = this->_url.find_last_of(".");
	if (i != std::string::npos)
	{
		std::string type = this->_url.substr(i + 1, this->_url.length());
		if (type == "css" || type == "js" || type == "html")
			this->_type = "text/" + type;
		else if (type == "jpg" || type == "png" || type == "jpeg" || type == "gif")
			this->_type = "image/" + type;
		else if (type == "mpeg" || type == "avi" || type == "mp4")
			this->_type = "video/" + type;
	}
}

void	Request::_parseRequestLine(void)
{
	//Parses method and put's it to string attribute _method, then erases it from the request
	
	const char* methods[3] = {"GET", "POST", "DELETE"};
	size_t i = this->_request.find_first_of(" ");
	this->_method = this->_request.substr(0, i);
	int index;
	
	for (index = 0; index < 3; index++)
	{
		if (this->_method == methods[index])
		{
			break;
		}
	}
	if (index == 3)
		this->_sanitizeStatus = 666;
	this->_request.erase(0, this->_method.length() + 1);
	
	//Parses URI and put's it to string attribute _url, then erases it from the request
	
	i = this->_request.find_first_of("?");
	if (i == std::string::npos)
		i = this->_request.find_first_of(" ");
	this->_url = this->_request.substr(0, i);
	this->_request.erase(0, this->_url.length());
	
	//Checks if there was query string attached to URI and if there was put's it to _queryString attribute, then erases it from the request 
	
	if (this->_request.find("?") == 0)
	{
		this->_request.erase(0, 1);
		i = this->_request.find_first_of(" ");
		this->_queryString = this->_request.substr(0, i);
		this->_request.erase(0, this->_queryString.length() + 1);
	}
	this->_getContentType();
	
	//Parses HTTP Version nd put's it to string attribute _httpVersion, then erases it from the request

	i = this->_request.find_first_of("\r\n");
	this->_httpVersion = this->_request.substr(0, i);
	this->_request.erase(0, this->_httpVersion.length() + 1);
}

//Parses headers line by line and adds the header(before :) and value (after :) to map container attribute _headers
void	Request::_parseHeaders(void)
{
	std::string line;

	size_t	lineEnd = 0;
	size_t	i = 0;

	while (line != "\r\n\r\n")
	{
		lineEnd = this->_request.find_first_of("\r\n");
		line = this->_request.substr(0, lineEnd);
		i = line.find_first_of(":");
		if (i == std::string::npos)
		{
			i = this->_request.find_last_of("\r\n");
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

void	Request::sanitize(void)
{
	if (this->_httpVersion != "HTTP/1.0" && this->_httpVersion != "HTTP/1.1")
	{
		this->_sanitizeStatus = 666;
		return;
	}
	if (this->_url.find("..") != std::string::npos)
	{
		this->_sanitizeStatus = 666;
		return;
	}
 	int i = this->_url.find_last_of("/");
	while (this->_url[i - 1] == '/')
	{
		this->_url.pop_back();
		i--;
	}
	if (this->_queryString.find_first_of("&;|`<>") != std::string::npos)
	{
		this->_sanitizeStatus = 666;
		return;
	}
	for (const auto& map_content : this->_headers)
	{
		if (map_content.first.find_first_of("&;|`<>()#") || map_content.first.length() > INT_MAX)
		{
			this->_sanitizeStatus = 666;
			break;
		}
		if (map_content.second.find_first_of("\r\n&;|`<>()#") != std::string::npos || map_content.second.length() > INT_MAX)
		{
			this->_sanitizeStatus = 666;
			break;
		}
	}
	if (this->_body.empty() == false)
	{
		try
		{
		 size_t len = std::stoi(this->_headers["Content-Length"]);
			if (this->_body.length() != len)
				this->_sanitizeStatus = 666;
		}
		catch(const std::exception& e)
		{
			this->_sanitizeStatus = 666;
		}
	}
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