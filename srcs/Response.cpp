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

void Response::respond(int clientfd)
{
	std::ifstream index;

	std::string response = "HTTP/1.1 200 OK\r\n";


	if (this->_url == "/")
	{
		this->_url = "/index.html";
		index.open("./www/index.html");
	}
	else
		index.open("./www" + this->_url);
	if (index.is_open() == false)
	{
		index.open("./www/404.html");
		response = "HTTP/1.1 404 Not Found\r\n";
		this->_url = "/404.html";
		this->_statusCode = 404;
	}
	else if (this->_statusCode != 200 && this->_statusCode != 404)
	{
		response = "HTTP/";
		response = this->_httpVersion + " ";
		response += getStatusMessage(this->_statusCode) + "\r\n";
	}
	if (this->_type.empty())
		this->_type = "text/html";
	response += "Content-Type: " + this->_type + "\r\n";
	if (this->_type == "video/mp4" || this->_type == "image/png")
		response += "Content-Disposition: attachment; filename=\"" + this->_url + "\r\n";
	response += "Content-Length: " + std::to_string(std::filesystem::file_size("./www" + this->_url)) + "\r\n";
	if (this->_statusCode == 200)
		response += "Keep-Alive: timeout=5, max=100\r\n\r\n";
	else
		response += "Connection: Close\r\n\r\n";

	send(clientfd, response.c_str(), response.length(), 0);

	const std::size_t chunkSize = 8192;
	char buffer[chunkSize];
	while (index.read(buffer, chunkSize) || index.gcount() > 0)
		send(clientfd, buffer, index.gcount(), 0);

	std::cout << "DONE " << std::filesystem::file_size("./www" + this->_url) << " " << "./www" + this->_url << std::endl;
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

