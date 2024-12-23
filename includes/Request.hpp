#pragma once

#include <unordered_map>
#include "ServerInfo.hpp"
#include <unistd.h>
#include <random>
#define MAX_URL_LENGTH 255
#define MAX_HEADER_LENGTH 255

class Request
{
	public:
		Request(void);
		Request(std::string request, int bodyLimit);
		virtual ~Request(void);
		Request(Request const& src);
		Request& operator=(Request const& src);

		void		parse(void);
		void		sanitize(ServerInfo server);
		std::string	getHost(void);
		std::string	getType(void) const;
		std::string	getContentLength(void);
		std::string	getQueryString(void) const;
		std::string	getMethod(void) const;
		std::string	getUrl(void) const;
		std::string	getRoot(void) const;
		std::string	getBody(void) const;
		std::string	getOrigLocLen(void) const;
		std::string	getConnectionHeader(void);
		std::string getSessionId (void) const;
		std::string getCookie(void);
		void 		setSessionId (std::string sessionId);
		void		printRequest(int clientSocket);
		void		openFile(ServerInfo server);
		void		openErrorFile(ServerInfo server, int sanitizeStatus);
		int			getFileFD() const;
		int			getSanitizeStatus( void ) const;
		void		setSanitizeStatus (int sanitizeStatus);

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
		std::string _sessionId;
		int			_filefd;
		std::unordered_map<std::string, std::string> _headers;

	private:
		bool		_checkAllowedMethods(ServerInfo server) const;
		void		_parseRequestLine(void);
		void 		_parseHeaders(void);
		void		_getContentType(void);
		void		_verifyPath(void);
		void		_parseMultipartContent(void);
		void		_parsePart(std::string& part);
		void		_decodeChunks(void);
		std::string	_splitMultiFormParts(std::string& boundary);
		std::string	_parseFileName(std::string& line);
		int			_bodyLimit;
};
