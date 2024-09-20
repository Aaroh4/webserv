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

void Response::respond(int clientfd, ServerInfo server)
{
	std::fstream file;
	std::streampos fsize = 0;

	(void) server; // THIS WILLLLLLLLLLLLLLLLLLLLLL BREAK THE CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	std::string response = "HTTP/1.1 200 OK\r\n";
	if (this->_url == "/")
	{
		//if (server.getlocationinfo()["/"].dirList == false)
		//{
			this->_url = "/index.html";
			file.open("./www/index.html");
		//}
		//else
		//{
		//	file = this->directorylist("./www" + this->_url);
		//	this->_url = "./dir.html";
		//}
	}
	else
	{
		//if (server.getlocationinfo()[this->_url].dirList == false)
			file.open("./www" + this->_url);
		//else
		//{
		//	file = this->directorylist("./www" + this->_url);
		//	this->_url = "./dir.html";
		//}
	}
	if (file.is_open() == false)
	{
		file.open("./www/404.html");
		response = "HTTP/1.1 404 Not Found\r\n";
		this->_url = "/404.html";
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
	response += "Keep-Alive: timeout=5, max=100\r\n\r\n";
	send(clientfd, response.c_str(), response.length(), 0);
	const std::size_t chunkSize = 8192;
	char buffer[chunkSize];
	while (file.read(buffer, chunkSize) || file.gcount() > 0) 
		send(clientfd, buffer, file.gcount(), 0);
	//std::cout << "DONE " << std::filesystem::file_size("./www" + this->_url) << " " << "./www" + this->_url << std::endl;
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



//<!DOCTYPE html>
//<html lang="en">
//<head>
//    <meta charset="UTF-8">
//    <meta name="viewport" content="width=device-width, initial-scale=1.0">
//    <link rel="icon" href="images/image.png" type="image/png">
//    <title>Cat site</title>
//    <link rel="stylesheet" href="styles.css">
//</head>
//<body>
//    <h1>Welcome to My Website</h1>
//	<img width="320" height="240" src="images/image.png" alt="Cat"> 
//	<video width="400" height="300" controls>
//		<source src="/video.mp4" type="video/mp4">
//	</video>
//</body>
//</html>