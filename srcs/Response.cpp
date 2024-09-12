#include "../includes/Response.hpp"

Response::Response(): Request()
{}

Response::Response(Request &request): Request(request)
{
}

Response::~Response()
{
}

Response::Response(const Response &input)
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
	if (this->_url == "/")
		index.open("./www/index.html");
	else
		index.open("./www" + this->_url);
	std::string file;
	for (std::string line; std::getline(index, line);)
		file += line;
	std::string response = "HTTP/1.1 200 OK\n";
	if (this->_url == "/styles.css")
		response += "Content-Type: text/css\n";
	else
		response += "Content-Type: text/html\n";
	response += "Content-Length: " + std::to_string(file.length()) + "\n";
	response += "Keep-Alive: timeout=5, max=100\n\n"; 
	response += file;
	send (clientfd, response.c_str(), response.length(), 0);
}
