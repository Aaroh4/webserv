#include "../includes/Response.hpp"

Response::Response(): Request()
{
	this->_fileSize = "0";
}

Response::Response(Request &request): Request(request)
{
	this->_fileSize = "0";
}

Response::~Response()
{
}

Response::Response(const Response &input): Request()
{
	*this = input;
}

Response Response::operator=(const Response &input){
	if (this != &input)
	{
		_server = input._server;
		_redirectplace =  input._redirectplace;
		_contentType = input._contentType;
		_response = input._response;
		_responseBody = input._responseBody;
		_errorMessage = input._errorMessage;
		_fileSize = input._fileSize;
	}
	return (*this);
}

void Response::handleCRUD(int clientfd, ServerInfo server)
{
	try{
		if (this->_type.rfind("cgi/", 0) == 0 && (this->_method == "GET" || this->_method == "POST"))
			cgiResponse(clientfd);
		else if (this->_method == "GET")
			respondGet(clientfd, server);
		else if (this->_method == "POST")
			respondPost(clientfd);
		else if (this->_method == "DELETE")
			respondDelete(clientfd);
		else
			throw ResponseException405();
	} catch (ResponseException &e){
		this->_sanitizeStatus = e.responseCode();
		sendErrorPage(e.responseCode(), clientfd, "", this->_sessionId.empty() ? "" : this->_sessionId);
	}
}

void	Response::respond(int clientfd, ServerInfo server)
{
	this->_server = server;
	try{
		switch (this->_sanitizeStatus){
			case 404:
				throw ResponseException404();
				break ;
			case 400:
				throw ResponseException400();
				break ;
			case 403:
				throw ResponseException403();
				break ;
			case 408:
				throw ResponseException408();
				break ;
			case 413:
				throw ResponseException413();
				break ;
			case 414:
				throw ResponseException414();
				break ;
			case 415:
				throw ResponseException415();
				break ;
			case 431:
				throw ResponseException431();
				break ;
			case 501:
				throw ResponseException501();
				break ;
			case 505:
				throw ResponseException505();
				break ;
			case 500:
				throw ResponseException();
				break ;
		}
	} catch(ResponseException& e) {
		sendErrorPage(e.responseCode(), clientfd,"", this->_sessionId.empty() ? "" : this->_sessionId);
		return ;
	}
	try{
		handleCRUD(clientfd, server);
	} catch(ResponseException& e){
		this->_sanitizeStatus = e.responseCode();
		sendErrorPage(e.responseCode(), clientfd, "", this->_sessionId.empty() ? "" : this->_sessionId);
		return ;
	}
}

std::string Response::formatSessionCookie( void ){

	std::string cookie = "Set-Cookie: " + this->_sessionId + "; HttpOnly; Path=/; Max-Age=60;\r\n";
	return cookie;
}

void Response::respondGet(int clientfd, ServerInfo server)
{
	std::string response;

	if (!server.getlocationinfo()[this->_origLoc].redirection.empty() || !server.getlocationinfo()[this->_url].redirection.empty())
	{
		this->_fileSize = "66";
		this->_sanitizeStatus = 302;
		this->_errorMessage = "Found";
		if (!server.getlocationinfo()[this->_url].redirection.empty())
			this->_redirectplace = server.getlocationinfo()[this->_url].redirection;
		else
			this->_redirectplace = server.getlocationinfo()[this->_origLoc].redirection;
		response = formatGetResponseMsg(0);
		std::cout << "response: " << response << std::endl;
		int res = send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
		if (res == -1)
			throw SendErrorException();
		if (res == 0)
			throw SendErrorException();
	}
	else if (server.getlocationinfo()[this->_origLoc].dirList != false && server.getlocationinfo()[this->_origLoc].index.empty()
        && std::filesystem::is_directory(this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos))
		&& this->_origLoc.back() == '/')
    {
        std::string tempdir = this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos);
        this->directorylisting(clientfd, this->buildDirectorylist(tempdir, tempdir.size()));
    }
	else
	{
		response = formatGetResponseMsg(0);
		std::cout << "response: " << response << std::endl;
		response += this->_responseBody;
		int res = send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
		if (res == -1)
			throw SendErrorException();
		if (res == 0)
			throw SendErrorException();
	}
}

void	Response::respondPost(int clientfd)
{
	std::string response;

	response = formatPostResponseMsg(0);
	int res = send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	if (res == -1)
		throw SendErrorException();
	if (res == 0)
		throw SendErrorException();
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;
}

std::string Response::formatPostResponseMsg (int close){

	std::string response;
	std::string timeOut = std::to_string(_server.get_timeout());

	if (timeOut.empty())
		timeOut = DEFAULT_TIMEOUT;
	if (this->_httpVersion.empty())
		this->_httpVersion = "HTTP/1.1";
	if (this->_body.empty() || this->_type.empty())
	{
		this->_sanitizeStatus = 204;
		response = this->_httpVersion + " 204 No Content\r\n";
	}
	else
	{
		setResponseBody(this->_body);
		response = this->_httpVersion + " 200 OK\r\n";
	}
	if (this->_sanitizeStatus == 200)
	{
		if (this->_type.empty())
			this->_type = "text/html";
		response += "Content-Type: " + this->_type + "\r\n";
		response += "Content-Length: " + std::to_string(this->_responseBody.length()) + "\r\n";
	}
	if (!this->_sessionId.empty())
		response += formatSessionCookie();
	if (close == 0)
	{
		response += "Connection: Keep-Alive\r\n";
		response += "Keep-Alive: timeout=" + timeOut + ", max=100\r\n\r\n";
	}
	if (this->_sanitizeStatus == 200)
		response += this->_responseBody;
	return (response);
}

void Response::cgiResponse(int clientfd)
{
	std::string response;
	std::string timeOut = std::to_string(_server.get_timeout());
	int res;

	if (timeOut.empty())
		timeOut = DEFAULT_TIMEOUT;

	if (this->_body.empty() && this->_method == "POST")
	{
		this->_sanitizeStatus = 204;
		response = this->_httpVersion + " 204 No Content\r\n";
		response += "Keep-Alive: timeout=" + timeOut + ", max=100\r\n\r\n";
	}
	else
		response = this->_responseBody;
	res = send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	if (res == -1)
		throw SendErrorException();
	if (res == 0)
		throw SendErrorException();
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;

}

void	Response::respondDelete(int clientfd)
{
	std::vector<std::string> supportedPaths = {
		this->_root + "/uploads"
	};
	std::string fileToDelete = this->_root + this->_url;
	bool canBeDeleted = false;
	for (const std::string &path : supportedPaths){
		if (fileToDelete.rfind(path,0) == 0){
			canBeDeleted = true;
			break ;
		}
	}
	if (!canBeDeleted)
		throw ResponseException403();
	std::filesystem::path file = fileToDelete;
	if (!std::filesystem::exists(file))
		throw ResponseException404();
	if (!std::filesystem::is_regular_file(file))
		throw ResponseException403();
	if (remove(fileToDelete.c_str()) != 0)
		throw ResponseException();
	std::string response = "HTTP/1.1 204 No Content\r\n";
	if (!this->_sessionId.empty())
		response += formatSessionCookie();
	response += "Connection: close\r\n\r\n";

	int res = send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	if (res == -1)
		throw SendErrorException();
	if (res == 0)
		throw SendErrorException();
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;
}

void Response::directorylisting(int clientfd, std::string file)
{
	std::string response;

	if (this->_type.empty())
		this->_type = "text/html";
	this->_fileSize = std::to_string(file.size());
	this->_responseBody = file;
	response = formatGetResponseMsg(0);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;
	response += file;
	int res = send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	if (res == -1)
		throw SendErrorException();
	if (res == 0)
		throw SendErrorException();
}

std::string Response::buildDirectorylist(std::string name, int rootsize)
{
	std::string	directory;
	directory += "<!DOCTYPE html>\n <html lang=\"en\">\n <head>\n </head>\n <body bgColor=\"#76ad0e\">\n";
	directory += " <h1>Directory listing<h1>\n <h1>[---------------------]</h1>\n <ol>\n";
	for (const std::filesystem::__cxx11::directory_entry & entry : std::filesystem::directory_iterator(name))
	{
		if (entry.is_directory())
			directory += "<li><a href=" + this->_url + entry.path().string().erase(0, rootsize) + "/>" + entry.path().string().erase(0, rootsize) + "/" + "</a> </li>" + "\n";
		else
			directory += "<li><a href=" + this->_url + entry.path().string().erase(0, rootsize) + ">" + entry.path().string().erase(0, rootsize) + "</a> </li>" + "\n";
	}
	directory += "</ol>\n <h1>[---------------------]</h1>\n </body>\n </html>\n";
	return (directory);
}

std::string Response::formatGetResponseMsg(int close)
{
	std::string response;
	std::string timeOut = std::to_string(_server.get_timeout());

	if (timeOut.empty())
		timeOut = DEFAULT_TIMEOUT;
	if (this->_httpVersion.empty())
		this->_httpVersion = "HTTP/1.1";

	if (this->_sanitizeStatus == 200)
		response = this->_httpVersion + " 200 OK\r\n";
	else
		response = this->_httpVersion + " " + std::to_string(this->_sanitizeStatus) + " " + this->_errorMessage +"\r\n";
	if (this->_type.empty())
		this->_type = "text/html";
	response += "Content-Type: " + this->_type + "\r\n";

	response += "Content-Length: " + std::to_string(this->_responseBody.length()) + "\r\n";
	if (!this->_redirectplace.empty())
		response += "Location: " + this->_redirectplace + "\r\n";
	response += formatSessionCookie();
	if (close == 0)
	{
		response += "Connection: keep-alive\r\n";
		response += "Keep-Alive: timeout=" + timeOut + ", max=100\r\n\r\n";
	}
	else
		response += "Connection: close\r\n\r\n";
	if (this->_sanitizeStatus == 302)
	{
		response += "<!DOCTYPE html>" \
			"<html>" \
			"<head>" \
			"<title>Redirecting...</title>" \
			"</head>" \
			"<body>" \
			"<p>This page has been moved</p>" \
			"</body>" \
			"</html>";
	}
	return (response);
}

std::string	makeErrorContent(int statusCode, std::string message)
{
	std::string content = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>";
	content += std::to_string(statusCode) + " " + message + "</title>\n<style>\n";
	content += "body {background-color: powderblue;}\n";
	content += "h1 {color: blue; font-style: italic; text-align: center;}\n</style>\n</head>\n<body>\n<h1>";
	content += std::to_string(statusCode) + " " + message + "</h1>\n</body>\n</html>\n";
	return content;
}

void	Response::setResponseBody(std::string body)
{
	this->_responseBody = body;
}

const char* Response::ResponseException::what() const noexcept{
	return "Internal Server Error";
}

int Response::ResponseException::responseCode() const{
	return (500);
}

const char* Response::ResponseException400::what() const noexcept{
	return "Bad Request";
}

int Response::ResponseException400::responseCode () const{
	return (400);
}

const char* Response::ResponseException403::what() const noexcept{
	return "Forbidden";
}

int Response::ResponseException403::responseCode () const{
	return (403);
}

const char* Response::ResponseException404::what() const noexcept{
	return "Not Found";
}

int Response::ResponseException404::responseCode () const{
	return (404);
}

const char* Response::ResponseException405::what() const noexcept{
	return "Method Not Allowed";
}

int Response::ResponseException405::responseCode () const{
	return (405);
}

const char* Response::ResponseException501::what() const noexcept{
	return "Unsupported method";
}

int Response::ResponseException501::responseCode () const{
	return (501);
}

const char* Response::ResponseException505::what() const noexcept{
	return "Unsupported HTTP";
}

int Response::ResponseException505::responseCode () const{
	return (505);
}

const char* Response::ResponseException415::what() const noexcept{
	return "Unsupported Media Type";
}

int Response::ResponseException415::responseCode () const{
	return (415);
}

const char* Response::ResponseException408::what() const noexcept{
	return "Request Timeout";
}

int Response::ResponseException408::responseCode () const{
	return (408);
}

const char* Response::ResponseException413::what() const noexcept{
	return "Content Too Large";
}

int Response::ResponseException413::responseCode () const{
	return (413);
}

const char* Response::ResponseException414::what() const noexcept{
	return "URI Too Long";
}

int Response::ResponseException414::responseCode () const{
	return (414);
}

const char* Response::ResponseException431::what() const noexcept{
	return "Request Header Fields Too Large";
}

int Response::ResponseException431::responseCode () const{
	return (431);
}

const char* Response::SendErrorException::what() const noexcept{
	return "Send failed";
}

void Response::sendErrorPage(int statusCode, int clientfd, std::string body, std::string cookie)
{
	std::string response;
	std::string fileSize;

	std::string message;
	switch (statusCode){
		case 400:
			message = "Bad Request";
			break ;
		case 403:
			message = "Forbidden";
			break ;
		case 405:
			message = "Method Not Allowed";
			break ;
		case 404:
			message = "Not Found";
			break ;
		case 408:
			message = "Request Timeout";
			break ;
		case 501:
			message = "Unsupported method";
			break ;
		case 505:
			message = "Unsupported HTTP";
			break ;
		case 415:
			message = "Unsupported Media Type";
			break ;
		case 413:
			message = "Content Too Large";
			break ;
		case 414:
			message = "URI Too Long";
			break ;
		case 431:
			message = "Request Header Fields Too Large";
			break ;
		default:
			message = "Internal Server Error";
	}
	if (body.empty())
	{
		body = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>";
		body += std::to_string(statusCode) + " " + message + "</title>\n<style>\n";
		body += "body {background-color: powderblue;}\n";
		body += "h1 {color: blue; font-style: italic; text-align: center;}\n</style>\n</head>\n<body>\n<h1>";
		body += std::to_string(statusCode) + " " + message + "</h1>\n</body>\n</html>\n";
	}

	fileSize = std::to_string(body.length());

	response = "HTTP/1.1 " + std::to_string(statusCode) + " " + message + "\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + fileSize + "\r\n";
	if (!cookie.empty())
		response += "Set-Cookie: " + cookie + "; HttpOnly; Path=/; Max-Age=60;\r\n";
	response += "Connection: close\r\n\r\n";

	std::string responseWithoutBody = response;
	response += body;

	int res = send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	if (res == -1)
		throw SendErrorException();
	if (res == 0)
		throw SendErrorException();
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << responseWithoutBody << std::endl;
}
