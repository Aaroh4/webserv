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

		virtual void		parse(void);
		virtual	void		sanitize(void);
		virtual int			getContentLength(void);
		virtual bool		isReceived(void) const;
		virtual void		appendBody(std::string& chunk);
		virtual std::unordered_map<std::string, std::string>	getHeaders(void) const;
	protected:
		int	_sanitizeStatus;
		bool _requestReceived;
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
		void	_verifyPath(void);
		void	_parsePostInput(void);
		void	_parsePart(std::string& part);
		void	_splitKeyValuePairs(void);
		void	_decodeChunks(void);
		std::string	_splitMultiFormParts(std::string& boundary);
		std::string	_parseFileName(std::string& line);
};

#endif
