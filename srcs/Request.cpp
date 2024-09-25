#include "../includes/Request.hpp"
#include <iostream>
#include <climits>
#include <filesystem>

Request::Request(void) : _sanitizeStatus(0)
{
}

Request::Request(std::string request) :  _sanitizeStatus(0), _request(request)
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
		this->_headers.clear();
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

	if (part.find("filename") == std::string::npos)
	{
		if (this->_formInput.empty() == false)
			this->_formInput += "&";
		start = part.find_first_of("\"");
		start++;
		end = part.find_first_of("\"", start);
		this->_formInput += part.substr(start, end - start) + "=";
		part.erase(0, end + 2);
		start = part.find_first_not_of("\r\n");
		end = part.find_last_of("\n");
		this->_formInput += part.substr(start, end - (start + 1));
	}
	else
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

void	Request::_parsePostInput(void)
{
	size_t		len = this->_headers["Content-Type"].length();
	size_t		ind = this->_headers["Content-Type"].find_first_of("=");
	size_t		lineEnd = 0;
	int			blockCount = 0;
	std::string	boundary = "--" + this->_headers["Content-Type"].substr(ind + 1, len - (ind + 1));
	std::string end = boundary + "--";
	std::string	value;
	std::string part;
	//std::string	line;
	//std::stringstream body(this->_body);

	this->_body.replace(0, boundary.length(), "");
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

void	Request::_runCgi(void)
{
	if (this->_headers["Content-Type"].find("multipart/form-data") != std::string::npos)
		this->_parsePostInput();
	else
		this->_formInput = this->_body;
	std::filesystem::path file = "./www" + this->_url;
	if (!std::filesystem::exists(file) || !std::filesystem::is_regular_file(file))
	{
		this->_sanitizeStatus = 666;
		return;
	}
	int permissions = access(file.c_str(), X_OK);
	if (permissions != 0)
	{
		this->_sanitizeStatus = 666;
		return;
	}
	//pid_t	pid = fork();

	//if (pid == 0)
	//{
	//	char* envp[100000];
	//	std::string url = "./www" + this->_url;
	//	std::string envVar0 = "REQUEST_METHOD=" + this->_method;
	//	std::string envVar1 = "CONTENT_LENGTH=" + this->_headers["Content-Length"];
	//	std::string envVar2 = "CONTENT_TYPE=" + this->_headers["Content-Type"];
	//	std::string envVar3 = "QUERY_STRING=" + this->_queryString;
	//	std::string envVar4 = "SCRIPT_FILENAME=" + url;
	//	std::string envVar5 = "GATEWAY_INTERFACE=CGI/1.1";
	//	std::string envVar6 = "SERVER_PROTOCOL=" + this->_httpVersion;
	//	std::string envVar7 = "SERVER_NAME=" + this->_headers["Host"];
	//	std::string envVar8 = "SERVER_PORT=8080";
	//	std::string envVar9 = "REMOTE_ADDR=192.168.0.1";

	//	if (this->_method == "GET")
	//	{
	//		envp = {
	//		envVar0.c_str(),
	//		envVar3.c_str(),
	//		envVar4.c_str(),
	//		envVar5.c_str(),
	//		envVar6.c_str(),
	//		envVar7.c_str(),
	//		envVar8.c_str(),
	//		envVar9.c_str(),
	//		nullptr
	//		};
	//	}
	//	else
	//	{
	//		envp = {
	//		envVar0.c_str(),
	//		envVar1.c_str(),
	//		envVar2.c_str(),
	//		envVar3.c_str(),
	//		envVar4.c_str(),
	//		envVar5.c_str(),
	//		envVar6.c_str(),
	//		envVar7.c_str(),
	//		envVar8.c_str(),
	//		envVar9.c_str(),
	//		nullptr
	//		};
	//	}
	//	char *args[2] = {url.c_str(), nullptr};
	//	if (execve(args[0], args, envp) < 0)
	//	{
	//		this->_sanitizeStatus = 666;
	//		return;
	//	}
	//}
	//else if (pid > 0)
	//	if (waitpid(pid, 0, 0) == -1)
	//	{
	//		this->_sanitizeStatus = 666;
	//		return;
	//	}
	//else
	//	this->_sanitizeStatus = 666;
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
		else if(type == "py")
			this->_runCgi();
    else if (this->_url != "/")
		  this->_sanitizeStatus = 415; // Error: Unsupported Media Type
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
		this->_sanitizeStatus = 666;
	this->_request.erase(0, this->_method.length() + 1);
	
	//Parses URI and put's it to string attribute _url, then erases it from the request
	
	i = this->_request.find_first_of("?");
	index = this->_request.find_first_of(" ");
	if (i < index)
		this->_url = this->_request.substr(0, i);
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
		this->_formInput = this->_request.substr(0, i);
		this->_request.erase(0, this->_formInput.length() + 1);
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

	//std::cout << "request before " << this->_request << std::endl; 
	while (line != "\r\n\r\n")
	{
		lineEnd = this->_request.find_first_of("\n");
		line = this->_request.substr(0, lineEnd + 1);
		i = line.find_first_of(":");
		if (i == std::string::npos)
		{
			i = this->_request.find_last_of("n\n");
			this->_request.erase(0, i + 1);
			break;
		}
		this->_headers[line.substr(0, i)] = line.substr(i + 2, lineEnd - (i + 3));
		this->_request.erase(0, lineEnd + 1);
	}
	//std::cout << "request after " << this->_request << std::endl; 
	this->_body = this->_request;
}

void	Request::parse(void)
{
	this->_parseRequestLine();
	this->_parseHeaders();
}

void	Request::sanitize(void)
{
	if (this->_httpVersion != "HTTP/1.0" && this->_httpVersion != "HTTP/1.1")
	{
		this->_sanitizeStatus = 505;
		return;
	}
	if (this->_url.find("..") != std::string::npos)
	{
		this->_sanitizeStatus = 666;
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
		this->_sanitizeStatus = 666;
		return;
	}
	for (const auto& map_content : this->_headers)
	{
		if (map_content.first.find_first_of("&;|`<>()#") != std::string::npos || map_content.first.length() > INT_MAX)
		{
			this->_sanitizeStatus = 666;
			break;
		}
	}
	if (this->_body.empty() == false)
	{
		try
		{
		 size_t len = std::stoi(this->_headers["Content-Length"]);
			if (this->_body.length() != len)
				this->_sanitizeStatus = 666;

		}
		catch(const std::exception& e)
		{
			this->_sanitizeStatus = 666;
		}
	}
}

std::string  Request::getMethod(void) const
{
	return this->_method;
}

std::string  Request::getUrl(void) const
{
	return this->_url;
}

std::string  Request::getBody(void) const
{
	return this->_body;
}

std::string  Request::getHttpVersion(void) const
{
	return this->_httpVersion;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return this->_headers;
}
