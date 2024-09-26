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
	if (this->_method == "GET")
		respondGet(clientfd, server);
	else if (this->_method == "POST")
		respondPost(clientfd, server);
	else if (this->_method == "DELETE")
		respondDelete(clientfd);
	else
		std::cout << "method not supported" << std::endl;
}

void	Response::respondDelete(int clientfd)
{
	std::string fileToDelete = "./www" + this->_url;
	if (remove(fileToDelete.c_str()) < 0)
		std::cout << "error\n";
	std::string response = "HTTP/1.1 200 OK\r\n";

	send (clientfd, response.c_str(), response.length(), 0);
}

void	Response::respondPost(int clientfd, ServerInfo server)
{
	this->handleCgi("." + server.getlocationinfo()["/" + cutFromTo(this->_url, 1, "/")].root + this->_url, clientfd);
}

void	Response::handleCgi(std::string path, int client_socket)
{
	int	pipefd[2];

	if (this->_type == "cgi/py" || this->_type == "cgi/php")
	{
		pipe(pipefd);
		int	pid = fork();
		if (pid == 0)
		{
			std::string request = "REQUEST_METHOD=" + this->_method;
			std::string query = "QUERY_STRING=" + this->_queryString;
			std::string length = "CONTENT_LENGTH=" + this->_headers["CONTENT_LENGTH"];
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
			std::cerr << path << std::endl;
			execve(path.c_str(), argv, envp);
			while (1)
			{
				std::cerr << "hello\n";
			}
			exit(0); // THIS NEEDS TO BE RemOVED BECAUSE ITS NOT ALLOWED EXCVE SHOULD BE USED INSTEAD
			// IF EXCVE FAILS YOU CAN THROW AN EXCEPTION INSTEAD OF USING EXIT
		}
		else 
		{  // Parent process
			// Close the write end of the pipe in the parent process
			close(pipefd[1]);

			// Read from the read end of the pipe (which contains the CGI output)
			char buffer[1024];
			ssize_t nbytes;
			while ((nbytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
				send(client_socket, buffer, nbytes, 0);  // Send CGI output to client
			}
			close(pipefd[0]);
			int	status;
			waitpid(pid, &status, 0);
    	}
	}
}

void Response::directorylisting(int clientfd, ServerInfo server, std::string file)
{
	(void) server; // THIS NEEDS TO BE CHANGED SINCE BEFORE TRYING ACCESS SOMETHING FROM IT
	std::string response = "HTTP/1.1 200 OK\r\n";
	if (this->_type.empty())
			this->_type = "text/html";
	response += "Content-Type: " + this->_type + "\r\n";
	response += "Content-Length: " + std::to_string(file.size()) + "\r\n";
	response += "Keep-Alive: timeout=5, max=100\r\n\r\n";
	response += file;
	send(clientfd, response.c_str(), response.length(), 0);
}

void Response::respondGet(int clientfd, ServerInfo server)
{
	std::fstream file;
	std::streampos fsize = 0;

	if (server.getlocationinfo()[this->_url].dirList != false)
	{
		std::cout << "root: " << server.getlocationinfo()[this->_url].root << std::endl;
		this->directorylisting(clientfd, server, this->directorylist(server.getlocationinfo()[this->_url].root, server.getlocationinfo()[this->_url].root.size()));
	}
	else
	{
		std::string response = "HTTP/1.1 200 OK\r\n";
		file.open("./" + server.getlocationinfo()["/" + cutFromTo(this->_url, 1, "/")].root + this->_url);
		//std::cout << "./" + server.getlocationinfo()["/" + cutFromTo(this->_url, 1, "/")].root + this->_url << std::endl;
		//std::cout << "url: " << this->_url << std::endl;
		//std::cout << "root: " << server.getlocationinfo()["/" + cutFromTo(this->_url, 1, "/")].root << std::endl;
		if (file.is_open() == false)
		{
			file.open("./" + server.getlocationinfo()[this->_url].root + "/" + server.getlocationinfo()[this->_url].index);
		}
		if (file.is_open() == false)
		{
			file.open("./www/404.html");
			response = "HTTP/1.1 404 Not Found\r\n";
			this->_url = "/404.html";
			this->_sanitizeStatus = 404;
		}

		if (this->_type.empty())
			this->_type = "text/html";
		response += "Content-Type: " + this->_type + "\r\n";
		if (this->_type == "video/mp4" || this->_type == "image/png")
			response += "Content-Disposition: attachment; filename=\"" + this->_url + "\r\n";
		file.seekg(0, std::ios::end);
		fsize = file.tellg();
		file.seekg(0, std::ios::beg);
		response += "Content-Length: " + std::to_string(fsize) + "\r\n";
		//std::cout << "statuscode: "<< this->_sanitizeStatus << std::endl;
		response += "Keep-Alive: timeout=5, max=100\r\n\r\n";

		send(clientfd, response.c_str(), response.length(), 0);

		const std::size_t chunkSize = 8192;
		char buffer[chunkSize];
		while (file.read(buffer, chunkSize) || file.gcount() > 0)
			send(clientfd, buffer, file.gcount(), 0);
	}
}

std::string Response::directorylist(std::string name, int rootsize)
{
	std::string	directory;
	//std::fstream directory("dir.html", std::ios::out | std::ios::in | std::ios::trunc);
	directory += "<!DOCTYPE html>\n <html lang=\"en\">\n <head>\n </head>\n <body bgColor=\"#76ad0e\">\n";
	directory += " <h1>Directory listing<h1>\n <h1>[---------------------]</h1>\n <ol>\n";
	for (const auto & entry : std::filesystem::directory_iterator(name))
	{
   		directory += "<li><a href=" + entry.path().string() + ">" + entry.path().string().erase(0, rootsize + 1) + "</a> </li>" + "\n";
 	}
	directory += "</ol>\n <h1>[---------------------]</h1>\n </body>\n </html>\n";
	return (directory);
}

std::string getStatusMessage(int statusCode)
{
	std::string statusMessage;

	statusMessage = std::to_string(statusCode);
	statusMessage += " ";
	switch(statusCode)
	{
		// 2XX Success responses

		case 200:
			statusMessage += "OK";
			break;
		case 201:
			statusMessage += "Created";
			break;
		case 202:
			statusMessage += "Accepted";
			break;
		case 203:
			statusMessage += "Non-Authoritative Information";
			break;
		case 204:
			statusMessage += "No Content";
			break;

		// 3XX Redirection responses

		case 301:
			statusMessage += "Moved Permanently";
			break;
		case 302:
			statusMessage += "Found";
			break;
		case 303:
			statusMessage += "See Other";
			break;
		case 304:
			statusMessage += "Not Modified";
			break;
		case 307:
			statusMessage += "Temporary Redirect";
			break;
		case 308:
			statusMessage += "Permanent Redirect";
			break;

		// 4XX Client error responses

		case 400:
			statusMessage += "Bad Request";
			break;
		case 401:
			statusMessage += "Unauthorized";
			break;
		case 403:
			statusMessage += "Forbidden";
			break;
		case 404:
			statusMessage += "Not Found";
			break;
		case 405:
			statusMessage += "Method Not Allowed";
			break;
		case 406:
			statusMessage += "Not Acceptable";
			break;
		case 408:
			statusMessage += "Request Timeout";
			break;
		case 413:
			statusMessage += "Payload Too Large";
			break;
		case 414:
			statusMessage += "URI Too Long";
			break;
		case 415:
			statusMessage += "Unsupported Media Type";
			break;
		case 418:
			statusMessage += "I'm a teapot";
			break;
		case 429:
			statusMessage += "Too Many Requests";
			break;
		case 431:
			statusMessage += "Request Header Fields Too Large";
			break;

		// 5XX Server error responses

		case 500:
			statusMessage += "Internal Server Error";
			break;
		case 501:
			statusMessage += "Not Implemented";
			break;
		case 502:
			statusMessage += "Bad Gateway";
			break;
		case 503:
			statusMessage += "Service Unavailable";
			break;
		case 505:
			statusMessage += "HTTP Version Not Supported";
			break;
		default:
			statusMessage += "Unknown Status Code";
			break;
	}
	return statusMessage;
}
