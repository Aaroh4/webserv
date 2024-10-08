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

void	Response::respond(int clientfd, ServerInfo server)
{
	this->_server = server;

	if (this->_sanitizeStatus != 200)
	{
		sendErrorResponse( clientfd );
		return;
	}
	if (this->_method == "GET")
		respondGet(clientfd, server);
	else if (this->_method == "POST")
		respondPost(clientfd, server);
	else if (this->_method == "DELETE")
		respondDelete(clientfd);
	else
	{
		this->_sanitizeStatus = 405; //Method not allowed
		std::string response = getStatusMessage(405);
		response += formatGetResponseMsg(1);
		send(clientfd, response.c_str(), response.length(), 0);
	}
}

void Response::respondGet(int clientfd, ServerInfo server)
{
	std::string response;

	std::string filePath = "./www" + this->_url;
	//std::cout << "index: " << server.getlocationinfo()[this->_url].index << " Url< " << this->_url << std::endl;
	if (server.getlocationinfo()[this->_url].dirList != false && server.getlocationinfo()[this->_url].index.empty())
	{
		std::cout << "directorylist: " << server.getlocationinfo()[this->_url].root + this->_url << std::endl;
		this->directorylisting(clientfd, this->buildDirectorylist(server.getlocationinfo()[this->_url].root + this->_url, server.getlocationinfo()[this->_url].root.size()));
	}
	else
	{
		try
		{
			openFile(filePath, server);
		}
		catch(ResponseException &e)
		{
			this->_errorMessage = e.what();
			sendErrorResponse(clientfd);
			std::cout << "?????????????????" << "\n";
			return;
		}
		response = formatGetResponseMsg(0);
		//std::cout << response << std::endl;
		send(clientfd, response.c_str(), response.length(), 0);
		const std::size_t chunkSize = 8192;
		char buffer[chunkSize];
		while (this->_file.read(buffer, chunkSize) || this->_file.gcount() > 0)
			send(clientfd, buffer, this->_file.gcount(), 0);
	}
}

void	Response::respondPost(int clientfd, ServerInfo server)
{	
	if (this->_type == "cgi/py" || this->_type == "cgi/php")
	{
		char result[4096];
		std::string location;

		ssize_t count = readlink("/proc/self/exe", result, 4096);

		location = std::string(result, (count > 0) ? count : 0);
		this->handleCgi(location.substr(0, location.rfind("/")) + server.getlocationinfo()["/" + cutFromTo(this->_url, 1, "/")].root + this->_url, clientfd);
	}
	else
	{
		std::string response;

		this->_sanitizeStatus = 204;
		response = formatGetResponseMsg(0);
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

void	Response::handleCgi(std::string path, int client_socket)
{
	int	pipefd[2];

	pipe(pipefd);
	int	pid = fork();
	if (pid == 0)
	{
		std::string request = "REQUEST_METHOD=" + this->_method;
		std::string query = "QUERY_STRING=param=value";
		std::string length = "CONTENT_LENGTH=" + this->_headers["Content-Length"];
		char *envp[] = {
			(char *) request.c_str(),
			(char *) query.c_str(),
			(char *) length.c_str(),
			nullptr
		};
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		char *argv[] = { (char*)path.c_str(), nullptr };
		execve(path.c_str(), argv, envp);
		exit(0); // THIS NEEDS TO BE RemOVED BECAUSE ITS NOT ALLOWED EXCVE SHOULD BE USED INSTEAD
		// IF EXCVE FAILS YOU CAN THROW AN EXCEPTION INSTEAD OF USING EXIT
	}
	else 
	{
		close(pipefd[1]);

		std::string response = "HTTP/1.1 204 No Content\r\n";
		response += "Content-Type: text/plain\r\n";
		//response += "Content-Length: " + std::to_string(file.size()) + "\r\n";
		response += "Keep-Alive: timeout=5, max=100\r\n\r\n";
		send(client_socket, response.c_str(), response.length(), 0);

		char buffer[1024];
		ssize_t nbytes;
		
		while ((nbytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) 
		{
			std::cout.write(buffer, nbytes).flush();
			//send(client_socket, buffer, nbytes, 0);
		}
		close(pipefd[0]);
		int	status;
		waitpid(pid, &status, 0);
	}
}

void Response::directorylisting(int clientfd, std::string file)
{
	std::string response;

	if (this->_type.empty())
			this->_type = "text/html";
	this->_fileSize = std::to_string(file.size());
	response = formatGetResponseMsg(0);
	response += file;
	send(clientfd, response.c_str(), response.length(), 0);
}

std::string Response::buildDirectorylist(std::string name, int rootsize)
{
	std::string	directory;
	directory += "<!DOCTYPE html>\n <html lang=\"en\">\n <head>\n </head>\n <body bgColor=\"#76ad0e\">\n";
	directory += " <h1>Directory listing<h1>\n <h1>[---------------------]</h1>\n <ol>\n";
	for (const auto & entry : std::filesystem::directory_iterator(name))
	{
		if (entry.is_directory())
			directory += "<li><a href=" + entry.path().string().erase(0, rootsize) + "/" + ">" + entry.path().string().erase(0, rootsize + this->_url.size()) + "/" + "</a> </li>" + "\n";
		else
			directory += "<li><a href=" + entry.path().string().erase(0, rootsize) + ">" + entry.path().string().erase(0, rootsize + this->_url.size()) + "</a> </li>" + "\n";
	}
	directory += "</ol>\n <h1>[---------------------]</h1>\n </body>\n </html>\n";
	return (directory);
}

void Response::openFile(std::string filePath, ServerInfo server)
{
	this->_fsize = 0;
	(void) filePath; // What to do with this??
	(void) server;
	std::string temp;
	std::string	test = "/" + cutFromTo(this->_url, 1, "/");


	while ((server.getlocationinfo()[temp].root.empty() || !server.getlocationinfo()[test].root.empty()) 
	&& test.size() + 1 <= this->_url.size())
	{
		temp = test;
		test += "/" + cutFromTo(this->_url, test.size() + 1, "/");
		if (!server.getlocationinfo()[test].root.empty())
			temp = test;
	}
	temp += "/";

	if (!server.getlocationinfo()[this->_url].index.empty())
		this->_file.open(server.getlocationinfo()[this->_url].root + "/" + server.getlocationinfo()[this->_url].index);
	else if (!server.getlocationinfo()[this->_url].root.empty())
		this->_file.open(server.getlocationinfo()[this->_url].root + this->_url);
	else
		this->_file.open(server.getlocationinfo()[temp].root + this->_url);


	if (this->_file.is_open() == false)
	{
		std::cout << errno << std::endl;
		switch errno
		{
			case EIO:
			case ENOMEM:
					this->_sanitizeStatus = 500; //internal error when I/O problem or no memory
					//throw ResponseException("Internal Server Error");
			case 21:	// might need its own error handling
			case ENOENT:
					this->_sanitizeStatus = 404;
					throw ResponseException();
			case EACCES:
					this->_sanitizeStatus = 403;
					//throw ResponseException("Forbidden");
		}
	}
	this->_file.seekg(0, std::ios::end);
	this->_fsize = this->_file.tellg();
	this->_fileSize = std::to_string(this->_fsize);
	this->_file.seekg(0, std::ios::beg);
}

std::string Response::formatGetResponseMsg(int close)
{
	std::string response;

	response = getStatusMessage(this->_sanitizeStatus);
	if (this->_type.empty())
		this->_type = "text/html";
	response += "Content-Type: " + this->_type + "\r\n";

	response += "Content-Length: " + this->_fileSize + "\r\n";
	
	if (close == 0)
		response += "Keep-Alive: timeout=" + this->_server.get_timeout() + ", max=100\r\n\r\n";
	else
		response += "Connection: close\r\n\r\n";
	return (response);
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
	std::string response;
	std::string file;

	this->_file.open("./www/404.html");
	this->_url = "/404.html";

	for (std::string line; std::getline(this->_file, line);)
		file += line;

	this->_fileSize = std::to_string(file.length());
	
	response = formatGetResponseMsg(0);
	response += file;	
	send(clientfd, response.c_str(), response.length(), 0);
}

void Response::sendCustomError(int clientfd)
{
	std::string response;

	this->_body = makeErrorContent(this->_sanitizeStatus, this->_errorMessage);
	this->_fileSize = std::to_string(this->_body.length());

	response = formatGetResponseMsg(0);
	response += this->_body;

	send(clientfd, response.c_str(), response.length(), 0);
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

const char* Response::ResponseException::what() const noexcept
{
	return "File not found\n";
}