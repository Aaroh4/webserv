#include "../includes/Request.hpp"
#include <iostream>
#include <climits>
#include <filesystem>

Request::Request(void) : _sanitizeStatus(0)
{
}

Request::Request(std::string request) :  _sanitizeStatus(200), _request(request)
{
	return;
}

Request::~Request(void)
{
	return;
}

Request::Request(Request const& src)
{
	this->_body = src._body;
	this->_httpVersion = src._httpVersion;
	this->_method = src._method;
	this->_type = src._type;
	this->_request = src._request;
	this->_url = src._url;
	this->_headers = src._headers;
	this->_sanitizeStatus = src._sanitizeStatus;
	this->_queryString = src._queryString;
	this->_root = src._root;
	this->_origLoc = src._origLoc;
}

Request& Request::operator=(Request const& src)
{
	if (this != &src)
	{
		this->_body = src._body;
		this->_httpVersion = src._httpVersion;
		this->_method = src._method;
		this->_type = src._type;
		this->_request = src._request;
		this->_url = src._url;
		this->_sanitizeStatus = src._sanitizeStatus;
		this->_queryString = src._queryString;
		this->_headers.clear();
		this->_root = src._root;
		this->_origLoc = src._origLoc;
		for (const auto& map_content : src._headers)
		{
			this->_headers[map_content.first] = map_content.second;
		}
	}
	return *this;
}

std::string Request::_splitMultiFormParts(std::string& boundary)
{
	size_t		i = this->_body.find(boundary);
	std::string part = this->_body.substr(0, i - 1);
	this->_body.erase(0, i + boundary.length() + 2);
	return part;
}

std::string	Request::_parseFileName(std::string& line)
{
	size_t start = 0;
	size_t end = 0;

	end = line.find_last_of("\"");
	for (start = end - 1; line[start] != '\"'; start--);
	start++;
	return (line.substr(start, end - start));
}

void	Request::_parsePart(std::string& part)
{
	size_t	start = 0;
	size_t	end = 0;
	std::ofstream newFile;

	if (part.find("filename") == std::string::npos) //If not a file build key - value pairs
	{
		if (this->_queryString.empty() == false)
			this->_queryString += "&";
		start = part.find_first_of("\"");
		start++;
		end = part.find_first_of("\"", start);
		this->_queryString += part.substr(start, end - start) + "=";
		part.erase(0, end + 2);
		start = part.find_first_not_of("\r\n");
		end = part.find_last_of("\n");
		this->_queryString += part.substr(start, end - (start + 1));
	}
	else	//Create newfile
	{
		end = part.find_first_of("\n");
		std::string line = part.substr(0, end);
		part.erase(0, end + 1);
		std::string	fileName = this->_parseFileName(line);
		newFile.open("./www/uploads/" + fileName, std::ios::binary);
		end = part.find_first_of("\n");
		part.erase(0, end + 1);
		start = part.find_first_not_of("\r\n");
		end = part.find_last_of("\n");
		newFile << part.substr(start, end - (start + 1));
	}
}

void	Request::_parseMultipartContent(void)
{
	size_t		len = this->_headers["Content-Type"].length();
	size_t		ind = this->_headers["Content-Type"].find_first_of("=");
	int			blockCount = 0;
	std::string	boundary = "--" + this->_headers["Content-Type"].substr(ind + 1, len - (ind + 1));
	std::string end = boundary + "--";
	std::string	value;
	std::string part;

	this->_body.erase(0, boundary.length() + 2);
	ind = 0;
	for (int i = 0; ind != std::string::npos; i++)
	{
		ind = this->_body.find(boundary, ind + 1);
		blockCount = i;
	}
	for (int i = 0; i < blockCount; i++)
	{
		part = this->_splitMultiFormParts(boundary);
		this->_parsePart(part);
	}
}

void	Request::_verifyPath(void)
{
	std::filesystem::path file = "./www" + this->_url;
	if (!std::filesystem::exists(file))
	{

		std::cout << "verifypath set 404" << std::endl;
		this->_sanitizeStatus = 404;
		this->_errmsg = "Not Found"; //Not Found
		return;
	}
	else if (!std::filesystem::is_regular_file(file))
	{
		this->_sanitizeStatus = 403;
		this->_errmsg = "Forbidden"; //Forbidden
		return;
	}
	int permissions = access(file.c_str(), X_OK);
	if (permissions != 0)
	{
		this->_sanitizeStatus = 403;
		this->_errmsg = "Forbidden"; //Forbidden
		return;
	}
}

void	Request::_getContentType(void)
{
	//Parses File type from url and set's the return content type to string attribute _type

	size_t	i = this->_url.find_last_of(".");
	if (i != std::string::npos)
	{
		std::string type = this->_url.substr(i + 1, this->_url.length());
		if (type == "css" || type == "js" || type == "html")
			this->_type = "text/" + type;
		else if (type == "jpg" || type == "png" || type == "jpeg" || type == "gif")
			this->_type = "image/" + type;
		else if (type == "mpeg" || type == "avi" || type == "mp4")
			this->_type = "video/" + type;
		else if(type == "py" || type == "php")
		{
			this->_verifyPath();
			this->_type = "cgi/" + type;
		}
    	else if (this->_url != "/")
		{
			this->_sanitizeStatus = 415;
			this->_errmsg = "Unsupported Media Type"; // Error: Unsupported Media Type
		}
	}
}

void	Request::_parseRequestLine(void)
{
	//Parses method and put's it to string attribute _method, then erases it from the request
	const char* methods[3] = {"GET", "POST", "DELETE"};
	size_t i = this->_request.find_first_of(" ");
	this->_method = this->_request.substr(0, i);
	size_t index;

	for (index = 0; index < 3; index++)
	{
		if (this->_method == methods[index])
		{
			break;
		}
	}
	if (index == 3)
	{
		this->_sanitizeStatus = 501;
		this->_errmsg = "Unsupported method";
	}
	this->_request.erase(0, this->_method.length() + 1);

	//Parses URI and put's it to string attribute _url, then erases it from the request

	i = this->_request.find_first_of("?");
	index = this->_request.find_first_of(" ");
	if (i < index)
	{
		this->_url = this->_request.substr(0, i);
		this->_request.erase(0, this->_url.length());
	}
	else
	{
		this->_url = this->_request.substr(0, index);
		this->_request.erase(0, this->_url.length() + 1);
	}

	//Checks if there was query string attached to URI and if there was put's it to _queryString attribute, then erases it from the request

	if (this->_request.find("?") == 0)
	{
		this->_request.erase(0, 1);
		i = this->_request.find_first_of(" ");
		this->_queryString = this->_request.substr(0, i);
		this->_request.erase(0, this->_queryString.length() + 1);
	}
	this->_getContentType();

	//Parses HTTP Version nd put's it to string attribute _httpVersion, then erases it from the request

	i = this->_request.find_first_of("\r\n");
	this->_httpVersion = this->_request.substr(0, i);
	this->_request.erase(0, this->_httpVersion.length() + 2);
}

void	Request::_parseHeaders(void)
{
	//Parses headers line by line and adds the header(before :) and value (after :) to map container attribute _headers
	std::string line;

	size_t	lineEnd = 0;
	size_t	i = 0;

	while (line != "\r\n\r\n")
	{
		lineEnd = this->_request.find_first_of("\n");
		line = this->_request.substr(0, lineEnd + 1);
		if (line == "\r\n")
		{
			this->_request.erase(0, 2);
			break;
		}
		i = line.find_first_of(":");
		if (i == std::string::npos)
		{
			this->_sanitizeStatus = 400;
			this->_errmsg = "Bad Request";
			break;
		}
		this->_headers[line.substr(0, i)] = line.substr(i + 2, lineEnd - (i + 3));
		this->_request.erase(0, lineEnd + 1);
	}
	this->_body = this->_request;
}

void	Request::_decodeChunks(void)
{
	int			chunkSize = 1;
	std::string line;
	std::string decodedBody;
	size_t 		pos = 0;
	int			totalSize = 0;

	while (chunkSize != 0)
	{
		pos = this->_body.find("\r\n");
		line = this->_body.substr(0, pos);
		this->_body.erase(0, pos + 2);
		try
		{
			chunkSize = std::stoi(line, nullptr, 16);
			totalSize += chunkSize;
			if (chunkSize == 0)
				break;
		}
		catch(const std::exception& e)
		{
			this->_sanitizeStatus = 500;
			this->_errmsg = "Internal Server Error";
			return;
		}
		decodedBody += this->_body.substr(0, chunkSize);
		this->_body.erase(0, chunkSize + 2);
	}
	if (totalSize != static_cast<int>(decodedBody.length()))
		std::cout << "paha\n";
	this->_body = decodedBody;
}

void	Request::parse(void)
{
	this->_parseRequestLine();
	this->_parseHeaders();
	if (this->_headers["Transfer-Encoding"] == "chunked")
		this->_decodeChunks();
	if (this->_headers["Content-Type"].find("multipart/form-data") != std::string::npos)
		this->_parseMultipartContent();
}

void	Request::sanitize(ServerInfo server)
{
	std::string temp;
	std::string	test = "/" + cutFromTo(this->_url, 1, "/");

	while ((server.getlocationinfo()[temp].root.empty()
		|| server.getlocationinfo()[test].root.empty()) && test.size() + 1 <= this->_url.size())
	{
		temp = test + "/";
		test += "/" + cutFromTo(this->_url, test.size() + 1, "/");
		if (!server.getlocationinfo()[test].root.empty())
			temp = test;
	}
	if (!server.getlocationinfo()[temp].root.empty())
	{
		this->_root = server.getlocationinfo()[temp].root;
		this->_origLoc = temp;
	}
	else
	{
		this->_root = server.getlocationinfo()["/"].root;
		this->_origLoc = "/";
		temp = "/";
	}
	if (this->_sanitizeStatus != 200)
		return ;
	if (this->_httpVersion != "HTTP/1.0" && this->_httpVersion != "HTTP/1.1")
	{
		this->_sanitizeStatus = 505;
		this->_errmsg = "Unsupported HTTP";
		return;
	}
	if (this->_url.find("..") != std::string::npos )
	{
		this->_sanitizeStatus = 403;
		this->_errmsg = "Forbidden"; //Forbidden
		return;
	}
 	int i = this->_url.find_last_of("/");
	while (this->_url[i - 1] == '/')
	{
		this->_url.pop_back();
		i--;
	}
	if (this->_queryString.find_first_of(";|`<>") != std::string::npos)
	{
		this->_sanitizeStatus = 400;
		this->_errmsg = "Bad Request"; //Bad request
		return;
	}
	for (const auto& map_content : this->_headers)
	{
		if (map_content.first.find_first_of("&;|`<>()#") != std::string::npos || map_content.first.length() > INT_MAX)
		{
			this->_sanitizeStatus = 400;
			this->_errmsg = "Bad Request"; //Bad request
			break;
		}
	}
}

std::string	Request::getHost(void)
{
	return this->_headers["Host"];
}

std::string	Request::getType(void)
{
	return this->_type;
}

std::string	Request::getContentLength(void)
{
	if (this->_headers.find("Content-Length") != this->_headers.end())
		return this->_headers["Content-Length"];
	return "";
}

std::string	Request::getQueryString(void)
{
	return this->_queryString;
}

std::string	Request::getMethod(void)
{
	return this->_method;
}

std::string	Request::getUrl(void)
{
	return this->_url;
}

std::string	Request::getRoot(void)
{
	return this->_root;
}

std::string	Request::getOrigLocLen(void)
{
	return this->_origLoc;
}

void Request::printRequest(int clientSocket)
{
	std::cout << "Client " << clientSocket << " Requested:\n";
	std::cout << "URL: "<< this->_url << std::endl;
	std::cout << "queryString: "<< this->_queryString << std::endl;
	std::cout << "Method: "<< this->_httpVersion << " " << this->_method << std::endl;
	std::cout << "Type: "<< this-> _type << std::endl;
	std::cout << "*******" << std::endl;
}
