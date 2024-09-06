/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:30:52 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/06 15:22:41 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H
# include <string>

class Request
{
	public:
		Request(std::string request);
		~Request(void);
		parseMethod(void);
		parseUrl(void);
	private:
		const std::string _request;
		const std::string _method;
		const std::string _url;
		const std::string _body;
}