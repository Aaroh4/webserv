/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkartasl <tkartasl@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 14:30:52 by tkartasl          #+#    #+#             */
/*   Updated: 2024/09/09 16:12:11 by tkartasl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H
# include <string>
# include <map>

class Request
{
	public:
		Request(std::string request);
		~Request(void);
		void	parseMethod(void);
		void	parseUrl(void);
		void parseHeaders(void);
	private:
		std::string _request;
		std::string _method;
		std::string _url;
		std::string _body;
		std::map<std::string, std::string> _headers;
};

#endif