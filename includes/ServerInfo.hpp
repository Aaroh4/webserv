#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include "libraries.hpp"
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

// Function for simplifying cutting string without having to mess around with substr
inline std::string cutFromTo(std::string input, int start, std::string last)
{
	input = input.substr(start, std::string::npos);
	return (input.substr(0, input.find(last)));
}

struct	location
{
	std::string									name;
	std::string									root;
	std::string									index;
	std::string									upload;
	bool										dirList = false;
	std::unordered_set<std::string>				methods;
};

class ServerInfo
{
	public:
			ServerInfo();
			ServerInfo(const ServerInfo &);
			ServerInfo operator=(const ServerInfo &);
			~ServerInfo();

			void												set_ip(std::string ip);
			unsigned int										get_ip() const;

			void												setnew_port(int port);
			int													get_port() const;

			void												setsocketfd(int fd);
			int													getsocketfd();

			void												setnewlocation(location input);
			std::unordered_map<std::string, location>			getlocationinfo() const;

			void												setTimeout(std::string input);
			std::string											get_timeout() const;

			void												setBodylimit(unsigned int input);
			unsigned int										getBodylimit() const;

			void												setServerName(std::string name);
			std::string											getServerName() const;
	private:
			std::string										_serverName;
			std::string										_timeout;
			unsigned int									_bodylimit;
			unsigned int									_ip;
			unsigned int									_port;
			std::unordered_map<std::string, location>		_locationinfo;
			int												_socketfd;
			std::string 									_locations;
			std::vector <struct pollfd>						_pollfds;
};

unsigned int convertip(std::string ip);

#endif