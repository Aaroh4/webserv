#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "libraries.hpp"
#include <unordered_map>
#include "ServerInfo.hpp"

class Request
{
	public:
		Request(void);
		Request(std::string request);
		virtual ~Request(void);
		Request(Request const& src);
		Request& operator=(Request const& src);

		void		parse(void);
		void		sanitize(ServerInfo server);
		std::string	getHost(void) const;
		std::string	getType(void) const;
		std::string	getContentLength(void) const;
		std::string	getQueryString(void) const;
		std::string	getMethod(void) const;
		std::string	getUrl(void) const;
		std::string	getRoot(void) const;
		std::string	getBody(void) const;
		std::string	getOrigLocLen(void) const;
		void		printRequest(int clientSocket);

	protected:
		int	_sanitizeStatus;
		std::string	_root;
		std::string _origLoc;
		std::string	_type;
		std::string _request;
		std::string _method;
		std::string _url;
		std::string _body;
		std::string _httpVersion;
		std::string _queryString;
		std::string _errmsg;
		std::unordered_map<std::string, std::string> _headers;
	private:
		void		_parseRequestLine(void);
		void 		_parseHeaders(void);
		void		_getContentType(void);
		void		_verifyPath(void);
		void		_parseMultipartContent(void);
		void		_parsePart(std::string& part);
		void		_decodeChunks(void);
		std::string	_splitMultiFormParts(std::string& boundary);
		std::string	_parseFileName(std::string& line);
};

#endif
