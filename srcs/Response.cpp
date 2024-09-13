#include "../includes/Response.hpp"
#include <filesystem>

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
	std::string response = "HTTP/1.1 200 OK\n";
	if (index.is_open() == false)
	{
		index.open("./www/404.html");
		response = "HTTP/1.1 404 Not Found\n";
	}
	std::string file;
	for (std::string line; std::getline(index, line);)
		file += line;
	if (this->_type.empty())
		response += "Content-Type: text/html\r\n";
	else
		response += "Content-Type: " + this->_type + "\r\n";
	if (this->_url == "/video.mp4" || this->_url == "/images/image.png")
	{
	   	std::ifstream index("./www" + this->_url);
		response += "Content-Disposition: attachment; filename=\"" + this->_url + "\"\r\n";
		response += "Content-Length: " + std::to_string(std::filesystem::file_size("./www" + this->_url)) + "\r\n";
		response += "Keep-Alive: timeout=5, max=100\r\n\r\n";

		send(clientfd, response.c_str(), response.length(), 0);
		
		const std::size_t chunkSize = 8192;
		char buffer[chunkSize];
		while (index.read(buffer, chunkSize) || index.gcount() > 0) 
			send(clientfd, buffer, index.gcount(), 0);

		std::cout << "DONE " << std::filesystem::file_size("./www" + this->_url) << " " << "./www" + this->_url << std::endl;
	}
	if (this->_url != "/video.mp4")
	{
		response += "Content-Length: " + std::to_string(file.length()) + "\r\n";
		response += "Keep-Alive: timeout=5, max=100\r\n\r\n"; 
		response += file;
		send (clientfd, response.c_str(), response.length(), 0);
	}
}
