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
		std::string	getHost(void);
		std::string	getType(void);
		std::string	getContentLength(void);
		std::string	getQueryString(void);
		std::string	getMethod(void);
		std::string	getUrl(void);
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
		//std::unordered_map<std::string, std::string> _data;
	private:
		void		_parseRequestLine(void);
		void 		_parseHeaders(void);
		void		_getContentType(void);
		void		_verifyPath(void);
		void		_parseMultipartContent(void);
		void		_parsePart(std::string& part);
		//void		_splitKeyValuePairs(void);
		void		_decodeChunks(void);
		std::string	_splitMultiFormParts(std::string& boundary);
		std::string	_parseFileName(std::string& line);
};

#endif
