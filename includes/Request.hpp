#ifndef REQUEST_H
# define REQUEST_H

#include "libraries.hpp"
#include <unordered_map>


class Request
{
	public:
		Request(void);
		Request(std::string request);
		~Request(void);
		Request(Request const& src);
		Request& operator=(Request const& src);

		void printparsedreq();
		virtual void	parse(void);
		virtual	void	sanitize(void);
		virtual std::string	getMethod(void) const;
		virtual std::string	getUrl(void) const;
		virtual std::string	getBody(void) const;
		virtual std::string	getHttpVersion(void) const;
		virtual int getContentLength(void);
		virtual std::unordered_map<std::string, std::string>	getHeaders(void) const;
	protected:
		int	_sanitizeStatus;
		std::string	_type;
		std::string _request;
		std::string _method;
		std::string _url;
		std::string _body;
		std::string _httpVersion;
		std::string _formInput;
		std::unordered_map<std::string, std::string> _headers;
		std::unordered_map<std::string, std::string> _data;
		void	_parseRequestLine(void);
		void 	_parseHeaders(void);
		void	_getContentType(void);
		void	_runCgi(void);
		void	_parsePostInput(void);
		void	_parsePart(std::string& part);
		void	_splitKeyValuePairs(void);
		std::string	_splitMultiFormParts(std::string& boundary);
		std::string	_parseFileName(std::string& line);
};

#endif
