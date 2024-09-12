/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:30:52 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/12 14:49:39 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H

#include "libraries.hpp"

class Request
{
	public:
		Request(void);
		Request(std::string request);
		~Request(void);
		Request(Request const& src);
		Request& operator=(Request const& src);
	
		virtual void parse(void);
		virtual std::string  getMethod(void) const;
		virtual std::string  getUrl(void) const;
		virtual std::string  getBody(void) const;
		virtual std::string  getHttpVersion(void) const;
		virtual std::map<std::string, std::string> getHeaders(void) const;
	protected:
		std::string	_type;
		std::string _request;
		std::string _method;
		std::string _url;
		std::string _body;
		std::string _httpVersion;
		std::map<std::string, std::string> _headers;
		void	_parseRequestLine(void);
		void 	_parseHeaders(void);
		void	_getContentType(void);
};

#endif