#include "../includes/ServerInfo.hpp"
#include "../includes/configExceptions.hpp"

ServerInfo::ServerInfo()
{}

ServerInfo::ServerInfo(const ServerInfo &input)
{
	this->_ip = input._ip;
	this->_port = input._port;
	this->_socketfd = input._socketfd;
	this->_locations = input._locations;
	this->_locationinfo = input._locationinfo;
	this->_timeout = input._timeout;
	this->_bodylimit = input._bodylimit;
}

ServerInfo ServerInfo::operator=(const ServerInfo &input)
{
	if (this != &input)
	{
		this->_ip = input._ip;
		this->_port = input._port;
		this->_socketfd = input._socketfd;
		this->_locations = input._locations;
		this->_locationinfo = input._locationinfo;
		this->_timeout = input._timeout;
		this->_bodylimit = input._bodylimit;
	}
	return (input);
}

ServerInfo::~ServerInfo()
{
}

void ServerInfo::set_ip(std::string ip)
{
	this->_ip = convertip(ip);
}
unsigned int		ServerInfo::get_ip() const
{
	return (this->_ip);
}

void	ServerInfo::setnew_port(int port)
{
	if (port > 65535)
		throw(wrongPort());
	this->_port = port;
}
			
			
int		ServerInfo::get_port() const
{
	return(this->_port);
}

void	ServerInfo::setsocketfd(int fd)
{
	this->_socketfd = fd;
}

int		ServerInfo::getsocketfd()
{
	return (this->_socketfd);
}

void	ServerInfo::setnewlocation(location input)
{
	this->_locationinfo[input.name] = input;
}

std::unordered_map<std::string, location>	ServerInfo::getlocationinfo() const
{
	return (this->_locationinfo);
}

void ServerInfo::setTimeout(std::string input)
{
	this->_timeout = input;
}

std::string	ServerInfo::get_timeout() const
{
	return (this->_timeout);
}

void			ServerInfo::setBodylimit(unsigned int input)
{
	this->_bodylimit = input;
}

unsigned int	ServerInfo::getBodylimit() const
{
	return (this->_bodylimit);
}