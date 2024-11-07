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

Response Response::operator=(const Response &input)
{
	if (this != &input)
	{

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
		std::cerr << e.what() << " in handleCRUD" << std::endl;
		this->_sanitizeStatus = e.responseCode();
		sendErrorResponse(e.what(), clientfd, e.responseCode(), server);
	}
}

void	Response::respond(int clientfd, ServerInfo server)
{
	this->_server = server;
	try{
		switch (this->_sanitizeStatus){
			case 404:
				throw ResponseException404();
			case 400:
				throw ResponseException400();
			case 403:
				throw ResponseException403();
			case 501:
				throw ResponseException501();
			case 505:
				throw ResponseException505();
			case 415:
				throw ResponseException415();
			case 500:
				throw ResponseException();
		}
	} catch(ResponseException& e) {
		std::cerr << e.what() << " in respond" << std::endl;
		this->_sanitizeStatus = e.responseCode();
		sendErrorResponse(e.what(), clientfd, e.responseCode(), server);
		return ;
	}
	try{
		handleCRUD(clientfd, server);
	} catch(ResponseException& e){
		std::cerr << e.what() << " in respond" << std::endl;
		this->_sanitizeStatus = e.responseCode();
		sendErrorResponse(e.what(), clientfd, e.responseCode(), server);
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
	}
	else if (server.getlocationinfo()[this->_origLoc].dirList != false && server.getlocationinfo()[this->_origLoc].index.empty()
        && std::filesystem::is_directory(this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos)))
    {
        std::string tempdir = this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos);
        this->directorylisting(clientfd, this->buildDirectorylist(tempdir, tempdir.size()));
    }
	else
	{
		response = formatGetResponseMsg(0);
		std::cout << "response: " << response << std::endl;
		send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
		send(clientfd, this->_responseBody.c_str(), this->_responseBody.length(), MSG_NOSIGNAL);
	}
}

void	Response::respondPost(int clientfd)
{
	std::string response;

	response = formatPostResponseMsg(1);
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;
}

std::string Response::formatPostResponseMsg (int close){

	std::string response;

	if (this->_httpVersion.empty())
		this->_httpVersion = "HTTP/1.1";
	if (this->_responseBody.empty())
	{
		this->_sanitizeStatus = 204;
		response = this->_httpVersion + " 204 No Content\r\n";
	}
	else
		response = this->_httpVersion + " 200 OK\r\n";
	this->_type = "text/html";
	if (this->_sanitizeStatus == 200)
		response += "Content-Type: " + this->_type + "\r\n";
	if (!this->_sessionId.empty())
		response += formatSessionCookie();
	if (close == 0)
	{
		response += "Connection: Keep-Alive\r\n";
		response += "Keep-Alive: timeout=5, max=100\r\n\r\n"; //this->_server.get_timeout()
	}
	else
		response += "Connection: close\r\n\r\n";
	if (!this->_responseBody.empty())
		response += this->_responseBody;
	return (response);

}

void Response::cgiResponse(int clientfd)
{
	std::string response;

	if (this->_httpVersion.empty())
		this->_httpVersion = "HTTP/1.1";
	if (this->_responseBody.empty())
	{
		this->_sanitizeStatus = 204;
		response = this->_httpVersion + " 204 No Content\r\n";
	}
		else
		response = this->_httpVersion + " 200 OK\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "Content-Length: " + std::to_string(this->_responseBody.length()) + "\r\n";
	if (!this->_sessionId.empty())
		response += formatSessionCookie();
	response += "Connection: Keep-Alive\r\n";
	response += "Keep-Alive: timeout=5, max=100\r\n\r\n"; //this->_server.get_timeout()
	response += this->_responseBody;
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
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
	response += "Connection: close\r\n\r\n";

	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
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
	std::string responseWithoutFile = response;
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	send(clientfd, file.c_str(), file.length(), MSG_NOSIGNAL);
}

std::string Response::buildDirectorylist(std::string name, int rootsize)
{
	std::string	directory;
	directory += "<!DOCTYPE html>\n <html lang=\"en\">\n <head>\n </head>\n <body bgColor=\"#76ad0e\">\n";
	directory += " <h1>Directory listing<h1>\n <h1>[---------------------]</h1>\n <ol>\n";
	for (const auto & entry : std::filesystem::directory_iterator(name))
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
	response += "Location: " + this->_redirectplace + "\r\n";
	response += formatSessionCookie();
	if (close == 0)
	{
		response += "Connection: keep-alive\r\n";
		response += "keep-alive: timeout=5, max=100\r\n\r\n"; //this->_server.get_timeout()
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

void Response::sendStandardErrorPage(int sanitizeStatus, int clientfd, ServerInfo server)
{
	std::string response;
	std::string file;
	(void) server;
	(void) sanitizeStatus;

	this->_fileSize = std::to_string(file.length());

	response = formatGetResponseMsg(1);
	std::string responseWithoutFile = response;
	if (this->_method == "GET")
		response += file;
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << responseWithoutFile << std::endl;

}

std::string Response::makeErrorContent(int statusCode, std::string message)
{
	std::string content = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>";
	content += std::to_string(statusCode) + " " + message + "</title>\n<style>\n";
	content += "body {background-color: powderblue;}\n";
	content += "h1 {color: blue; font-style: italic; text-align: center;}\n</style>\n</head>\n<body>\n<h1>";
	content += std::to_string(statusCode) + " " + message + "</h1>\n</body>\n</html>\n";
	return content;
}

void Response::sendCustomErrorPage(int clientfd)
{
	std::string response;

	this->_body = makeErrorContent(this->_sanitizeStatus, this->_errorMessage);
	this->_fileSize = std::to_string(this->_body.length());

	response = formatGetResponseMsg(0);
	response += this->_body;

	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;
}

void Response::sendErrorResponse(std::string errorMessage, int clientfd, int errorCode, ServerInfo server)
{
	this->_errorMessage = errorMessage;
	this->_sanitizeStatus = errorCode;
	if (this->_sanitizeStatus == 400 ||
		this->_sanitizeStatus == 405 ||
		this->_sanitizeStatus == 403 ||
		this->_sanitizeStatus == 500 ||
		this->_sanitizeStatus == 404 ||
		this->_sanitizeStatus == 505 ||
		this->_sanitizeStatus == 501 ||
		this->_sanitizeStatus == 415)
	{
		sendStandardErrorPage(this->_sanitizeStatus, clientfd, server);
		return ;
	}
	else
		sendCustomErrorPage(clientfd);
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


void Response::sendErrorPage(int statusCode, int clientfd, std::string body)
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
		case 501:
			message = "Unsupported method";
			break ;
		case 505:
			message = "Unsupported HTTP";
			break ;
		case 415:
			message = "Unsupported Media Type";
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
	response += "Connection: close\r\n\r\n";

	std::string responseWithoutBody = response;
	response += body;

	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << responseWithoutBody << std::endl;
}
