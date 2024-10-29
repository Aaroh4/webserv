#include "../includes/Response.hpp"
#include <filesystem>
#include <sys/wait.h>

Response::Response(): Request()
{
}

Response::Response(Request &request): Request(request)
{
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
		if (this->_method == "GET")
			respondGet(clientfd, server);
		else if (this->_method == "POST")
			respondPost(clientfd, server);
		else if (this->_method == "DELETE")
			respondDelete(clientfd);
		else
			throw ResponseException405();
	} catch (const ResponseException &e){
		sendErrorResponse(e.what(), clientfd, e.responseCode());
	}
}

void	Response::respond(int clientfd, ServerInfo server)
{
	this->_server = server;
	try{
		switch (this->_sanitizeStatus){
			//case 404:
			//	throw ResponseException404();
			//case 400:
			//	throw ResponseException400();
			case 403:
				throw ResponseException403();
			//case 501:
			//	throw ResponseException501();
			//case 505:
			//	throw ResponseException505();
			//case 515:
			//	throw ResponseException515();
			//case 500:
			//	throw ResponseException();
		}
	} catch(const ResponseException& e) {
		sendErrorResponse(e.what(), clientfd, e.responseCode());
		return ;
	}
	try{
		handleCRUD(clientfd, server);
	} catch(const ResponseException& e){
		sendErrorResponse(e.what(), clientfd, e.responseCode());
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

	if (server.getlocationinfo()[this->_url].dirList != false && server.getlocationinfo()[this->_url].index.empty())
	{
		this->directorylisting(clientfd, this->buildDirectorylist(server.getlocationinfo()[this->_url].root, server.getlocationinfo()[this->_url].root.size() + 1));
	}
	else
	{
		try
		{
			openFile(server);
		}
		catch(ResponseException &e)
		{
			sendErrorResponse(e.what(), clientfd, e.responseCode());
			return;
		}
		response = formatGetResponseMsg(0);
		send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
		const std::size_t chunkSize = 8192;
		char buffer[chunkSize];
		while (this->_file.read(buffer, chunkSize) || this->_file.gcount() > 0)
			send(clientfd, buffer, this->_file.gcount(), MSG_NOSIGNAL);
	}
	response = formatGetResponseMsg(0);
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;
	const std::size_t chunkSize = 8192;
	char buffer[chunkSize];
	while (this->_file.read(buffer, chunkSize) || this->_file.gcount() > 0)
		send(clientfd, buffer, this->_file.gcount(), MSG_NOSIGNAL);
}

void	Response::respondPost(int clientfd, ServerInfo server)
{
	std::string response;

	(void) server;
	response = formatPostResponseMsg(1);
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << response << std::endl;

	std::cout << "this should be the responsebody: " << this->_responseBody << std::endl;
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
	response += formatSessionCookie();
	if (close == 0)
	{
		response += "Connection: Keep-Alive\r\n";
		response += "Keep-Alive: timeout=" + this->_server.get_timeout() + ", max=100\r\n\r\n";
	}
	else
		response += "Connection: close\r\n\r\n";
	if (!this->_responseBody.empty())
		response += this->_responseBody;
	return (response);

}

void	Response::respondDelete(int clientfd)
{
	std::vector<std::string> supportedPaths = {
		"./www/uploads/"
		};
	std::string fileToDelete = "./www" + this->_url;
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
	response = formatGetResponseMsg(0);
	std::string responseWithoutFile = response;
	response += file;
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	//std::cout << "Response to client: " << clientfd << std::endl;
	//std::cout << responseWithoutFile << std::endl;
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

void Response::openFile(ServerInfo server)
{
	this->_fsize = 0;

	if (!server.getlocationinfo()[this->_url].index.empty())
		this->_file.open(server.getlocationinfo()[this->_url].root + "/" + server.getlocationinfo()[this->_url].index);
	else if (!this->_root.empty())
		this->_file.open(this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos));
	else
		this->_file.open(server.getlocationinfo()["/"].root + "/" + this->_url);


	if (this->_file.is_open() == false)
	{
		switch errno
		{
			case EIO:
			case ENOMEM:
					this->_sanitizeStatus = 500;
					throw ResponseException(); //internal error when I/O problem or no memory might need some logging;
			case ENOTDIR:
			case ENOENT:
					this->_sanitizeStatus = 404;
					throw ResponseException404();
			case EACCES:
					this->_sanitizeStatus = 403;
					throw ResponseException403();
			default:
					this->_sanitizeStatus = 500;
					throw ResponseException();		}
	}
	this->_file.seekg(0, std::ios::end);
	this->_fsize = this->_file.tellg();
	this->_fileSize = std::to_string(this->_fsize);
	this->_file.seekg(0, std::ios::beg);
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

	response += "Content-Length: " + this->_fileSize + "\r\n";
	response += formatSessionCookie();
	if (close == 0)
	{
		response += "Connection: keep-alive\r\n";
		response += "keep-alive: timeout=" + this->_server.get_timeout() + ", max=100\r\n\r\n";
	}
	else
		response += "Connection: close\r\n\r\n";
	return (response);
}

void Response::sendStandardErrorPage(int sanitizeStatus, int clientfd)
{
	std::string response;
	std::string file;

	switch (sanitizeStatus)
	{
		case 400:
			this->_file.open("./www/400.html");
			this->_url = "/400.html";
			break ;
		case 403:
			this->_file.open("./www/403.html");
			this->_url = "/403.html";
			break ;
		case 405:
			this->_file.open("./www/405.html");
			this->_url = "/405.html";
			break ;
		case 404:
			this->_file.open("./www/404.html");
			this->_url = "/404.html";
			break ;
		case 501:
			this->_file.open("./www/501.html");
			this->_url = "/501.html";
			break ;
		case 505:
			this->_file.open("./www/505.html");
			this->_url = "/505.html";
			break ;
		case 515:
			this->_file.open("./www/515.html");
			this->_url = "/515.html";
			break ;
		default:
			this->_file.open("./www/500.html");
			this->_url = "/500.html";
			break ;
	}
	for (std::string line; std::getline(this->_file, line);)
		file += line;

	this->_fileSize = std::to_string(file.length());

	response = formatGetResponseMsg(1);
	std::string responseWithoutFile = response;
	if (this->_method == "GET")
		response += file;
	send(clientfd, response.c_str(), response.length(), MSG_NOSIGNAL);
	std::cout << "Response to client: " << clientfd << std::endl;
	std::cout << responseWithoutFile << std::endl;

}

std::string makeErrorContent(int statusCode, std::string message)
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

void Response::sendErrorResponse(std::string errorMessage, int clientfd, int errorCode)
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
		this->_sanitizeStatus == 515)
	{
		sendStandardErrorPage(this->_sanitizeStatus, clientfd);
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

const char* Response::ResponseException515::what() const noexcept{
	return "Unsupported Media Type";
}

int Response::ResponseException515::responseCode () const{
	return (515);
}
