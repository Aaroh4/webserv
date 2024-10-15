#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerInfo.hpp"
#include "libraries.hpp"
#include <algorithm>
#include <unordered_map>
#define DEFAULT "test1.cfg"

class ServerManager
{
	public:
			ServerManager();
			ServerManager(const ServerManager &);
			ServerManager operator=(const ServerManager &);
			~ServerManager();

			int		startServers();
			void	setNewInfo(ServerInfo server);
			void	runServers();
			void	addNewConnection(size_t i);
			void	receiveRequest(size_t& i);
			size_t	getRequestLength(std::string& request);
			size_t	findLastChunk(std::string& request, size_t start_pos);
			void	handleRequest(std::string& request, int clienSocket);
			void	removeConnection(int clientSocket, size_t& i);
			std::vector<ServerInfo> get_info();
	private:
			std::vector<ServerInfo> _info;
			std::vector<struct pollfd> _poll_fds;
			std::unordered_map<int, int> _connections;
};

#endif
