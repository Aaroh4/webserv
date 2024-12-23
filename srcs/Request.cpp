#include "../includes/Request.hpp"
#include <climits>
#include "../includes/Response.hpp"
#include <fcntl.h>

Request::Request(void) : _sanitizeStatus(200)
{
	this->_filefd =  0;
}

Request::Request(std::string request, int bodyLimit) :  _sanitizeStatus(200), _request(request), _bodyLimit(bodyLimit)
{
	this->_filefd =  0;
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
	this->_sessionId = src._sessionId;
	this->_bodyLimit = src._bodyLimit;
	this->_filefd = src._filefd;
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
		this->_sessionId = src._sessionId;
		this->_bodyLimit = src._bodyLimit;
		this->_filefd = src._filefd;
		this->_headers = src._headers;
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
		newFile << part.substr(start, std::string::npos);
	}
}

void	Request::_parseMultipartContent(void)
{
	size_t		len = this->_headers["Content-Type"].length();
	size_t		ind = this->_headers["Content-Type"].find_first_of("=");
	int			blockCount = 0;
	std::string	boundary = "--" + this->_headers["Content-Type"].substr(ind + 1, len - (ind + 1));
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
	std::filesystem::path file = this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos);
	if (!std::filesystem::exists(file))
	{
		this->_sanitizeStatus = 404;
		throw Response::ResponseException404();
	}
	else if (!std::filesystem::is_regular_file(file))
	{
		this->_sanitizeStatus = 403;
		throw Response::ResponseException403();
	}
	int permissions = access(file.c_str(), X_OK);
	if (permissions != 0)
	{
		this->_sanitizeStatus = 403;
		throw Response::ResponseException403();
	}
}

void	Request::_getContentType(void)
{
	//Parses File type from url and set's the return content type to string attribute _type
	size_t	i = this->_url.find_last_of(".");
	if (i != std::string::npos)
	{
		std::string type = this->_url.substr(i + 1, this->_url.length());
		if (type == "css" || type == "js" || type == "html" || type == "txt")
			this->_type = "text/" + type;
		else if (type == "jpg" || type == "png" || type == "jpeg" || type == "gif")
			this->_type = "image/" + type;
		else if (type == "mpeg" || type == "avi" || type == "mp4")
			this->_type = "video/" + type;
		else if (type == "py" || type == "php")
			this->_type = "cgi/" + type;
    	else if (this->_url != "/")
		{
			this->_sanitizeStatus = 404;
			this->_errmsg = "Not Found";
			throw Response::ResponseException404();
		}
	}
}

void	Request::_parseRequestLine(void)
{
	//Parses method and put's it to string attribute _method, then erases it from the request
	size_t i = this->_request.find_first_of(" ");
	this->_method = this->_request.substr(0, i);
	size_t index;

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
	if (this->_url.length() > MAX_URL_LENGTH)
	{
		throw Response::ResponseException414();
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

	//Parses HTTP Version and put's it to string attribute _httpVersion, then erases it from the request

	i = this->_request.find_first_of("\r\n");
	this->_httpVersion = this->_request.substr(0, i);
	this->_request.erase(0, this->_httpVersion.length() + 2);
	if (this->_httpVersion != "HTTP/1.1")
	{
		throw Response::ResponseException505();
	}
}

void	Request::_parseHeaders(void)
{
	//Parses headers line by line and adds the header(before :) and value (after :) to map container attribute _headers
	if (this->_request.find_first_not_of("\r\n") == std::string::npos)
		return;
	std::string line;
	std::stringstream request(this->_request);
	size_t	i = 0;
	std::string key;
	std::string value;

	while (line != "\r\n\r\n")
	{
		std::getline(request, line);
		i = line.find_first_of(":");
		if (i == std::string::npos)
			break ;
		key = line.substr(0, i);
		value = line.substr(i + 2, (line.length() - (i + 3)));
		if (key.length() > MAX_HEADER_LENGTH || value.length() > MAX_HEADER_LENGTH)
		{
			throw Response::ResponseException431();
		}
		this->_headers[key] = value;
	}
	size_t bodyStart = this->_request.find("\r\n\r\n") + 4;
	if (bodyStart != std::string::npos)
	{
		this->_body = this->_request.substr(bodyStart, std::string::npos);
	}
	if (this->_bodyLimit > 0 && static_cast<int> (this->_body.length()) > this->_bodyLimit)
		throw Response::ResponseException413();
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
			throw Response::ResponseException();
		}
		decodedBody += this->_body.substr(0, chunkSize);
		this->_body.erase(0, chunkSize + 2);
	}
	if (totalSize != static_cast<int>(decodedBody.length()))
	{
	  	this->_body = decodedBody;
		this->_sanitizeStatus = 500;
		throw Response::ResponseException();
	}
}

std::string generateSessionId( void ){
	//generate a 16 random numbers between 0-255 using mt19937 randomnumber gen.
	std::random_device randomSeed;
	std::mt19937 generator(randomSeed());
	std::uniform_int_distribution<int> dist(0, 255);

	unsigned char buf[16];
	for (unsigned long i = 0; i < sizeof(buf); i++){
		buf[i] = dist(generator);
	}

	//makes the 16 random bytes to hexa values
	std::stringstream ss;
	for (unsigned long i = 0; i < sizeof(buf); i++){
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)buf[i];
	}
	return ("session_id=" + ss.str());
}

void	Request::parse(void)
{
	try
	{
		this->_parseRequestLine();
		this->_parseHeaders();
		if (this->_headers["Transfer-Encoding"] == "chunked")
			this->_decodeChunks();
		if (this->_headers["Content-Type"].find("multipart/form-data") != std::string::npos)
			this->_parseMultipartContent();
		if (this->_headers.find("Cookie") != this->_headers.end())
			this->setSessionId(this->_headers["Cookie"]);
		else
			this->setSessionId(generateSessionId());
	}
	catch(Response::ResponseException& e)
	{
		throw;
	}
}

bool	Request::_checkAllowedMethods(ServerInfo server) const
{
	if (server.getlocationinfo()[this->_origLoc].methods.find(this->_method) == server.getlocationinfo()["this->_origloc"].methods.end())
		return false;
	return true;
}

void	Request::sanitize(ServerInfo server)
{
	std::string temp;
	std::string	test = "/" + cutFromTo(this->_url, 1, "/")  + "/";

	try{
		if (!server.getlocationinfo()[test].root.empty())
			temp = test;

		while (test.size() + 1 <= this->_url.size()){
			test += cutFromTo(this->_url, test.size(), "/") + "/";
			if (!server.getlocationinfo()[test].root.empty())
				temp = test;
		}
		if (!server.getlocationinfo()[temp].root.empty()){
			this->_root = server.getlocationinfo()[temp].root;
			this->_origLoc = temp;
		}
		else{
			this->_root = server.getlocationinfo()["/"].root;
			this->_origLoc = "/";
			temp = "/";
		}
		if ((this->_url.back() != '/' && std::filesystem::is_directory(this->_root + "/" + this->_url.substr(this->_origLoc.size() - 1, std::string::npos)))
		|| (server.getlocationinfo()[_origLoc].dirList == false && server.getlocationinfo()[_origLoc].index.empty()))
			throw Response::ResponseException404();
		if (this->_checkAllowedMethods(server) == false)
			throw Response::ResponseException405();
		if (this->_type == "cgi/py" || this->_type == "cgi/php")
			this->_verifyPath();
		if (this->_sanitizeStatus != 200)
			return ;
		if (this->_url.find("..") != std::string::npos )
		{
			this->_sanitizeStatus = 403;
			throw Response::ResponseException403();
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
			throw Response::ResponseException400();
		}
		for (const std::unordered_map<std::string, std::string>::iterator::value_type& map_content : this->_headers)
		{
			if (map_content.first.find_first_of("&;|`<>()#") != std::string::npos || map_content.first.length() > INT_MAX)
			{
				this->_sanitizeStatus = 400;
				throw Response::ResponseException400();
			}
		}
	} catch (std::exception &e){
		throw ;
	}
}

void Request::openFile(ServerInfo server)
{
	try{
		if (this->_origLoc.size() > this->_url.size())
		{
			std::string location = this->_url;
			std::filesystem::path absolutePath = std::filesystem::absolute("www" + location);
			if (std::filesystem::is_directory(absolutePath) == true)
			{
				if (static_cast<std::string>(absolutePath).back() != '/')
					throw Response::ResponseException404();
				return;
			}
		}
		else if (!(!server.getlocationinfo()[this->_origLoc].redirection.empty() || !server.getlocationinfo()[this->_url].redirection.empty())
		&& !(server.getlocationinfo()[this->_origLoc].dirList != false && server.getlocationinfo()[this->_origLoc].index.empty()
			&& std::filesystem::is_directory(this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos))))
		{
			if (!server.getlocationinfo()[this->_url].index.empty())
				this->_filefd = open((server.getlocationinfo()[this->_url].root + "/" + server.getlocationinfo()[this->_url].index).c_str(), O_RDONLY);
			else if (!this->_root.empty())
				this->_filefd = open((this->_root + "/" + this->_url.substr(this->_origLoc.size() - 1, std::string::npos)).c_str(), O_RDONLY);
			else
				this->_filefd = open((server.getlocationinfo()["/"].root + "/" + this->_url).c_str(), O_RDONLY);

			if (this->_filefd < 0)
			{
				switch errno
				{
					case ENOTDIR:
					case ENOENT:
					case 21:
							this->_sanitizeStatus = 404;
							throw Response::ResponseException404();
					case EACCES:
							this->_sanitizeStatus = 403;
							throw Response::ResponseException403();
					default:
							this->_sanitizeStatus = 500;
							throw Response::ResponseException();
				}
			}
	}
	}catch (Response::ResponseException &e)
	{
		throw ;
	}
	catch (std::exception &e)
	{
		throw Response::ResponseException404();
	}
}

void Request::openErrorFile(ServerInfo server, int sanitizeStatus)
{
	if (this->_filefd != 0)
		close (this->_filefd);
	switch (sanitizeStatus)
	{
		case 400:
			if (!server.getErrorPages()[400].empty())
				this->_filefd = open(server.getErrorPages()[400].c_str(), O_RDONLY);
			else
				this->_filefd = open("./www/403.html", O_RDONLY);
			break ;
		case 403:
			if (server.getErrorPages()[403].empty())
				this->_filefd = open("./www/403.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[403].c_str(), O_RDONLY);
			break ;
		case 404:
			if (server.getErrorPages()[404].empty())
				this->_filefd = open("./www/404.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[404].c_str(), O_RDONLY);
			break ;
		case 405:
			if (server.getErrorPages()[405].empty())
				this->_filefd = open("./www/405.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[405].c_str(), O_RDONLY);
			break ;
		case 408:
			if (server.getErrorPages()[408].empty())
				this->_filefd = open("./www/408.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[408].c_str(), O_RDONLY);
			break ;
		case 413:
			if (server.getErrorPages()[413].empty())
				this->_filefd = open("./www/413.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[413].c_str(), O_RDONLY);
			break ;
		case 414:
			if (server.getErrorPages()[414].empty())
				this->_filefd = open("./www/414.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[414].c_str(), O_RDONLY);
			break ;
		case 415:
			if (server.getErrorPages()[415].empty())
				this->_filefd = open("./www/415.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[415].c_str(), O_RDONLY);
			break ;
		case 431:
			if (server.getErrorPages()[431].empty())
				this->_filefd = open("./www/431.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[431].c_str(), O_RDONLY);
			break ;
		case 501:
			if (server.getErrorPages()[501].empty())
				this->_filefd = open("./www/501.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[501].c_str(), O_RDONLY);
			break ;
		case 505:
			if (server.getErrorPages()[505].empty())
				this->_filefd = open("./www/505.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[505].c_str(), O_RDONLY);
			break ;
		default:
			if (server.getErrorPages()[500].empty())
				this->_filefd = open("./www/500.html", O_RDONLY);
			else
				this->_filefd = open(server.getErrorPages()[500].c_str(), O_RDONLY);
			break ;
	}
}

std::string	Request::getHost(void)
{
	return this->_headers["Host"];
}

std::string	Request::getType(void) const
{
	return this->_type;
}

std::string	Request::getContentLength(void)
{
	if (this->_headers.find("Content-Length") != this->_headers.end())
		return this->_headers["Content-Length"];
	return "";
}

std::string	Request::getQueryString(void) const
{
	return this->_queryString;
}

std::string	Request::getMethod(void) const
{
	return this->_method;
}

std::string	Request::getUrl(void) const
{
	return this->_url;
}

std::string	Request::getRoot(void) const
{
	return this->_root;
}

std::string	Request::getBody(void) const
{
	return this->_body;
}

std::string	Request::getOrigLocLen(void) const
{
	return this->_origLoc;
}

std::string	Request::getConnectionHeader(void)
{
	return this->_headers["Connection"];
}
std::string	Request::getCookie(void)
{
	if (this->_headers.empty())
		return "";
	if (this->_headers.find("Cookie") != this->_headers.end())
		return this->_headers["Cookie"];
	return "";
}

std::string	Request::getSessionId(void) const
{
	return this->_sessionId;
}

void Request::setSessionId (std::string sessionId){
	this->_sessionId = sessionId;
}

int Request::getSanitizeStatus(void) const{
	return this->_sanitizeStatus;
}

void Request::setSanitizeStatus (int sanitizeStatus){
	this->_sanitizeStatus = sanitizeStatus;
}

void Request::printRequest(int clientSocket)
{
	std::cout << "Client " << clientSocket << " Requested:\n";
	std::cout << "URL: "<< this->_url << std::endl;
	std::cout << "queryString: "<< this->_queryString << std::endl;
	std::cout << "Method: "<< this->_httpVersion << " " << this->_method << std::endl;
	std::cout << "Type: "<< this-> _type << std::endl;
	std::cout << "Session ID: " << this->_sessionId << std::endl;
	std::cout << "*******" << std::endl;
}

int	Request::getFileFD() const
{
	return (this->_filefd);
}
