#include "../includes/ServerInfo.hpp"
#include "configExceptions.cpp"

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
	this->_serverName = input._serverName;
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
		this->_serverName = input._serverName;
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
	try {
		this->_timeout = std::stoi(input);
	} catch (std::exception& e) {
		throw(wrongTimeout());
	}
}

unsigned int	ServerInfo::get_timeout() const
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

void ServerInfo::setServerName(std::string name)
{
	this->_serverName = name;
}

std::string	ServerInfo::getServerName() const
{
	return (this->_serverName);
}

void ServerInfo::setErrorPage(int error, std::string page)
{
	this->_errorPages[error] = page;
}

std::unordered_map<int, std::string>	ServerInfo::getErrorPages() const
{
	return (this->_errorPages);
}
