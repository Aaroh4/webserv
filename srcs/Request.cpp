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

	std::cout << "part is " << part << std::endl;
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
		std::cout << "end here is " << end << std::endl;
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
	std::filesystem::path file = this->_root + "/" + this->_url.substr(this->_origLoc.size(), std::string::npos);
	if (!std::filesystem::exists(file))
	{
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
		if (type == "css" || type == "js" || type == "html")
			this->_type = "text/" + type;
		else if (type == "jpg" || type == "png" || type == "jpeg" || type == "gif")
			this->_type = "image/" + type;
		else if (type == "mpeg" || type == "avi" || type == "mp4")
			this->_type = "video/" + type;
		else if (type == "py" || type == "php")
			this->_type = "cgi/" + type;
    	else if (this->_url != "/")
		{
			//std::cerr << "Unsupported Media Type in _getContentType" << std::endl;
			this->_sanitizeStatus = 404;
			this->_errmsg = "Not Found";
			throw Response::ResponseException404();
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
		throw Response::ResponseException501();
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
	std::stringstream request(this->_request);
	size_t	i = 0;

	while (line != "\r\n\r\n")
	{
		std::getline(request, line);
		i = line.find_first_of(":");
		if (i == std::string::npos)
			break ;
		this->_headers[line.substr(0, i)] = line.substr(i + 2, (line.length() - (i + 3)));
	}
	size_t bodyStart = this->_request.find("\r\n\r\n") + 4;
	size_t bodyEnd = this->_request.find_last_of("\r\n\r\n") + 4;
	this->_body = this->_request.substr(bodyStart, bodyEnd);
	if (this->_bodyLimit > 0 && static_cast<int> (this->_body.length()) > this->_bodyLimit)
		throw Response::ResponseException400();
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
			std::cerr << e.what() << " stoi failed in _decodeChunks" << std::endl;
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

		if (this->_type == "cgi/py" || this->_type == "cgi/php")
			this->_verifyPath();

		if (this->_sanitizeStatus != 200)
			return ;
		if (this->_httpVersion != "HTTP/1.0" && this->_httpVersion != "HTTP/1.1")
		{
			this->_sanitizeStatus = 505;
			throw Response::ResponseException505();
		}
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
		for (const auto& map_content : this->_headers)
		{
			if (map_content.first.find_first_of("&;|`<>()#") != std::string::npos || map_content.first.length() > INT_MAX)
			{
				this->_sanitizeStatus = 400;
				throw Response::ResponseException400();
			}
		}
	} catch (Response::ResponseException &e){
			std::cerr << e.what() << " in sanitize"<< std::endl;
			throw ;
		}
}

void Request::openFile(ServerInfo server)
{
	if (!(!server.getlocationinfo()[this->_origLoc].redirection.empty() || !server.getlocationinfo()[this->_url].redirection.empty())
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

std::string	Request::getSessionId(void) const
{
	return this->_sessionId;
}

void Request::setSessionId (std::string sessionId){
	this->_sessionId = sessionId;
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

int	Request::getFileFD()
{
	return (this->_filefd);
}
