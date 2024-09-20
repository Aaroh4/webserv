#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerInfo.hpp"
#include "libraries.hpp"
#include <algorithm> 
#include <unordered_map>

class ServerManager
{
	public:
			ServerManager();
			ServerManager(const ServerManager &);
			ServerManager operator=(const ServerManager &);
			~ServerManager();

			void	start_servers();
			void	setnew_info(ServerInfo server);
			std::vector<ServerInfo> get_info();
	private:
			std::vector<ServerInfo> _info;
};

#endif