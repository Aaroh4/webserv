/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:30:52 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/12 13:29:40 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H

#include "libraries.hpp"

class Request
{
	public:
		Request(std::string request);
		~Request(void);
	
		virtual void parse(void);
		void respond(int clientfd);
		virtual std::string  getMethod(void) const;
		virtual std::string  getUrl(void) const;
		virtual std::string  getBody(void) const;
		virtual std::string  getHttpVersion(void) const;
		virtual std::map<std::string, std::string> getHeaders(void) const;
	private:
		std::string	_type;
		std::string _request;
		std::string _method;
		std::string _url;
		std::string _body;
		std::string _httpVersion;
		std::map<std::string, std::string> _headers;
		void	_parseRequestLine(void);
		void 	_parseHeaders(void);
};

#endif