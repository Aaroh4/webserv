#include "../includes/Response.hpp"
#include <filesystem>

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

void	Response::respond(int clientfd, ServerInfo server)
{
	if (this->_sanitizeStatus != 200)
	{
		sendErrorResponse( clientfd );
		return;
	}
	//std::cout << "method: "<< this->_method << std::endl;
	if (this->_method == "GET")
		respondGet(clientfd, server);
	else if (this->_method == "POST")
		respondPost(clientfd);
	else if (this->_method == "DELETE")
		respondDelete(clientfd);
	else
	{
		this->_sanitizeStatus = 405; //Method not allowed
		std::string response = getStatusMessage(405);
		response += "Content-Type: text/html\r\n";
		response += "Connection: close\r\n\r\n";
		send(clientfd, response.c_str(), response.length(), 0);
	}
}

void	Response::respondDelete(int clientfd)
{

	std::string fileToDelete = "./www" + this->_url;
	if (remove(fileToDelete.c_str()) < 0)
		std::cout << "error\n";
	std::string response = "HTTP/1.1 200 OK\r\n";

	send (clientfd, response.c_str(), response.length(), 0);
}

void	Response::respondPost(int clientfd)
{
	(void)clientfd;
	/*if (index.is_open() == false)
	{
		index.open("./www/404.html");
		response = "HTTP/1.1 404 Not Found\r\n";
	}*/
}

// void Response::respondGet(int clientfd, ServerInfo server)
// {
// 	std::fstream file;
// 	std::streampos fsize = 0;

// 	(void) server; // THIS WILLLLLLLLLLLLLLLLLLLLLL BREAK THE CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// 	std::string response = "";

// 	std::string filePath = "./www" + this->_url;
// 	if (this->_url == "/")
// 	{
// 		//if (server.getlocationinfo()["/"].dirList == false)
// 		//{
// 			this->_url = "/index.html";
// 			file.open("./www/index.html");
// 		//}
// 		//else
// 		//{
// 		//	file = this->directorylist("./www" + this->_url);
// 		//	this->_url = "./dir.html";
// 		//}
// 	}
// 	else
// 	{
// 		//if (server.getlocationinfo()[this->_url].dirList == false)
// 		if (access(filePath.c_str(), R_OK) != 0)
// 		{
// 			std::cout << "error: " << errno << std::endl;
// 			this->_sanitizeStatus = 403; //Forbidden
// 		}
// 		else
// 			file.open(filePath);
// 		//else
// 		//{
// 		//	file = this->directorylist("./www" + this->_url);
// 		//	this->_url = "./dir.html";
// 		//}
// 	}
// 	if (file.is_open() == false)
// 	{
// 		if (errno == EIO || errno == ENOMEM)
// 		{
// 			this->_sanitizeStatus = 500; //internal error when I/O problem or no memory
// 		}
// 		file.open("./www/404.html");
// 		this->_sanitizeStatus = 404;
// 		this->_url = "/404.html";
// 	}
// 	response = formatGetResponseMsg();
// 	file.seekg(0, std::ios::end);
// 	fsize = file.tellg();
// 	this->_fileSize = std::to_string(fsize);
// 	file.seekg(0, std::ios::beg);


// 	response = formatGetResponseMsg();
// 	send(clientfd, response.c_str(), response.length(), 0);

// 	const std::size_t chunkSize = 8192;
// 	char buffer[chunkSize];
// 	while (file.read(buffer, chunkSize) || file.gcount() > 0)
// 		send(clientfd, buffer, file.gcount(), 0);
// }

void Response::respondGet(int clientfd, ServerInfo server)
{
	std::streampos fsize = 0;

	(void) server; // THIS WILLLLLLLLLLLLLLLLLLLLLL BREAK THE CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	std::string response = "";

	if (this->_url == "/")
		this->_url = "/index.html";
	std::string filePath = "./www" + this->_url;
	try{
		openFile(filePath);
	}
	catch(ResponseException &e)
	{
		this->_errorMessage = e.what();
		sendErrorResponse(clientfd);
		return;
	}
	response = formatGetResponseMsg();
	send(clientfd, response.c_str(), response.length(), 0);
	const std::size_t chunkSize = 8192;
	char buffer[chunkSize];
	while (this->_file.read(buffer, chunkSize) || this->_file.gcount() > 0)
		send(clientfd, buffer, this->_file.gcount(), 0);
}

void Response::openFile(std::string filePath)
{
	this->_fsize = 0;
	this->_file.open(filePath);
	if (this->_file.is_open() == false)
	{
		if (errno == EIO || errno == ENOMEM)
		{
			this->_sanitizeStatus = 500; //internal error when I/O problem or no memory
			throw ResponseException("Internal Server Error");
		}
		else if (errno == ENOENT)
		{
			this->_sanitizeStatus = 404;
			throw ResponseException("Not Found");
		}
		else if (errno == EACCES)
		{
			this->_sanitizeStatus = 403;
			throw ResponseException("Forbidden");
		}
	}
	this->_file.seekg(0, std::ios::end);
	this->_fsize = this->_file.tellg();
	this->_fileSize = std::to_string(this->_fsize);
	this->_file.seekg(0, std::ios::beg);
}

std::fstream Response::directorylist(std::string name)
{
	//std::string	directory;
	std::fstream directory("dir.html", std::ios::out | std::ios::in | std::ios::trunc);
	directory << "<!DOCTYPE html>\n <html lang=\"en\">\n <head>\n </head>\n <body>\n <ol>\n";
	for (const auto & entry : std::filesystem::directory_iterator(name))
	{
   		directory << "<li><a href=" << entry.path().string().erase(0, 6) << ">" << entry.path() << "</a> </li>" << std::endl;
 	}
	directory << "</ol>\n <h1>Welcome to My Website</h1>\n </body>\n </html>\n";
	return (directory);
}

std::string Response::formatGetResponseMsg( void )
{
	std::string response;

	response = getStatusMessage(this->_sanitizeStatus);
	if (this->_type.empty())
		this->_type = "text/html";
	response += "Content-Type: " + this->_type + "\r\n";
	if (this->_type == "video/mp4" || this->_type == "image/png")
		response += "Content-Disposition: attachment; filename=\"" + this->_url + "\r\n";
	response += "Content-Length: " + this->_fileSize + "\r\n";
	response += "Keep-Alive: timeout=5, max=100\r\n\r\n";
	return response;
}

std::string Response::getStatusMessage(int statusCode)
{
	std::string statusMessage;
	std::string message;

	statusMessage = this->_httpVersion + " ";
	statusMessage += std::to_string(this->_sanitizeStatus);
	statusMessage += " ";
	switch(statusCode)
	{
		// 2XX Success responses

		case 200:
			message += "OK";
			break;
		case 201:
			message += "Created";
			break;
		case 202:
			message += "Accepted";
			break;
		case 203:
			message += "Non-Authoritative Information";
			break;
		case 204:
			message += "No Content";
			break;

		// 3XX Redirection responses

		case 301:
			message += "Moved Permanently";
			break;
		case 302:
			message += "Found";
			break;
		case 303:
			message += "See Other";
			break;
		case 304:
			message += "Not Modified";
			break;
		case 307:
			message += "Temporary Redirect";
			break;
		case 308:
			message += "Permanent Redirect";
			break;

		// 4XX Client error responses

		case 400:
			message += "Bad Request";
			break;
		case 401:
			message += "Unauthorized";
			break;
		case 403:
			message += "Forbidden";
			break;
		case 404:
			message += "Not Found";
			break;
		case 405:
			message += "Method Not Allowed";
			break;
		case 406:
			message += "Not Acceptable";
			break;
		case 408:
			message += "Request Timeout";
			break;
		case 413:
			message += "Payload Too Large";
			break;
		case 414:
			statusMessage += "URI Too Long";
			break;
		case 415:
			message += "Unsupported Media Type";
			break;
		case 418:
			message += "I'm a teapot";
			break;
		case 429:
			message += "Too Many Requests";
			break;
		case 431:
			message += "Request Header Fields Too Large";
			break;

		// 5XX Server error responses

		case 500:
			message += "Internal Server Error";
			break;
		case 501:
			message += "Not Implemented";
			break;
		case 502:
			message += "Bad Gateway";
			break;
		case 503:
			message += "Service Unavailable";
			break;
		case 505:
			message += "HTTP Version Not Supported";
			break;
		default:
			message += "Unknown Status Code";
			break;
	}
	statusMessage += message;
	statusMessage += "\r\n";
	this->_errorMessage = message;
	return statusMessage;
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

void Response::sendNotFound(int clientfd)
{
	this->_file.open("./www/404.html");
	this->_url = "/404.html";
	this->_file.seekg(0, std::ios::end);
	this->_fsize = this->_file.tellg();
	this->_fileSize = std::to_string(this->_fsize);
	this->_file.seekg(0, std::ios::beg);
	
	std::string response = getStatusMessage(404);
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + this->_fileSize + "\r\n";
	response += "Keep-Alive: timeout=5, max=100\r\n\r\n";
	
	send(clientfd, response.c_str(), response.length(), 0);
	
	const std::size_t chunkSize = 8192;
	char buffer[chunkSize];
	while (this->_file.read(buffer, chunkSize) || this->_file.gcount() > 0)
		send(clientfd, buffer, this->_file.gcount(), 0);
}

void Response::sendCustomError(int clientfd)
{
	std::string response = getStatusMessage(this->_sanitizeStatus);

	this->_body = makeErrorContent(this->_sanitizeStatus, this->_errorMessage);
	this->_fileSize = std::to_string(this->_body.length());

	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + this->_fileSize + "\r\n";
	response += "Keep-Alive: timeout=5, max=100\r\n\r\n";

	send(clientfd, response.c_str(), response.length(), 0);
	send(clientfd, this->_body.c_str(), this->_body.length(), 0);
}

void Response::sendErrorResponse( int clientfd )
{
	if (this->_sanitizeStatus == 404)
	{
		sendNotFound(clientfd);
		return ;
	}
	else
		sendCustomError(clientfd);
}

Response::ResponseException::ResponseException(const std::string& message): _message(message)
{
}

Response::ResponseException::~ResponseException() noexcept
{
}

const char* Response::ResponseException::what() const noexcept
{
	return _message.c_str();
}




