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

		virtual void	parse(void);
		virtual	void	sanitize(void);
		virtual std::string	getMethod(void) const;
		virtual std::string	getUrl(void) const;
		virtual std::string	getBody(void) const;
		virtual std::string	getHttpVersion(void) const;
		virtual std::map<std::string, std::string>	getHeaders(void) const;
	protected:
		int	_sanitizeStatus;
		std::string	_type;
		std::string _request;
		std::string _queryString;
		std::string _method;
		std::string _url;
		std::string _body;
		std::string _httpVersion;
		std::string _formInput;
		std::map<std::string, std::string> _headers;
		void	_parseRequestLine(void);
		void 	_parseHeaders(void);
		void	_getContentType(void);
		void 	_parseQueryString(void);
		void	_runCgi(void);
		void	_parsePostInput(void);
		void	_parseBlock(std::string& block);
		std::string	_splitMultiFormBlocks(std::string& boundary);
		std::string	_parseFileName(std::string line);
};

#endif
