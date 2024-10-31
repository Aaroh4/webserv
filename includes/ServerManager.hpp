#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "ServerInfo.hpp"
#include "libraries.hpp"
#include <algorithm>
#include <unordered_map>
#include "Request.hpp"
#define DEFAULT "test1.cfg"

typedef struct s_clientInfo
{
	Request*	req;
	int			pipeFd;
	std::string	request;
	std::string cgiResponseBody;
	bool		requestReceived;
	bool		cgiResponseReady;
	int			responseStatus;
	std::time_t	latestRequest;
}	t_clientInfo;

class ServerManager
{
	public:
			ServerManager();
			ServerManager(const ServerManager &);
			ServerManager operator=(const ServerManager &);
			~ServerManager();

			int		startServers(void);
			void	setNewInfo(ServerInfo server);
			void	runServers(void);
			void	runCgi(std::string path, char** envp, int& clientSocket);
			void	addNewConnection(size_t& i);
			void	receiveRequest(size_t& i);
			size_t	getRequestLength(std::string& request);
			size_t	findLastChunk(std::string& request, size_t start_pos);
			void	sendResponse(size_t& i);
			void	cleanPreviousRequestData(int clientSocket, size_t& i);
			void	addPipeFd(int pipeFd);
			int		checkForCgi(Request& req, int& clientSocket);
			bool	isPipeFd(int& fd);
			void	readFromCgiFd(const int& fd);
			bool	checkConnectionUptime(int& clientSocket);
			void	closeConnection(int& clientSocket, size_t& i);
			std::vector<ServerInfo> get_info(void);
	private:
			std::unordered_map<int, t_clientInfo>  	_clientInfos; //key = client socket
			std::vector<ServerInfo>					_info;
			std::vector<struct pollfd> 				_poll_fds;
			std::unordered_map<int, int> 			_connections; // clients and servers
			std::unordered_map<int, int> 			_clientPipe; //key = pipe fd & value = client socket
};

#endif

