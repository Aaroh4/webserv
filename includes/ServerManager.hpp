#pragma once

#include "ServerInfo.hpp"
#include <algorithm>
#include <unordered_map>
#include "Request.hpp"
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <poll.h>
#include <fstream>
#include <arpa/inet.h>
#include <fcntl.h>
#define DEFAULT "test1.cfg"
#define TIMEOUT 900.0f

typedef struct s_clientInfo
{
	Request*	req;
	int			pipeFd;
	std::string	request;
	std::string ResponseBody;
	bool		requestReceived;
	bool		ResponseReady;
	int			responseStatus;
	size_t		requestLength;
	bool		failedToReceiveRequest;
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
			size_t	getRequestLength(std::string& request, int& clientSocket, size_t& i);
			size_t	findLastChunk(std::string& request, size_t start_pos);
			void	sendResponse(size_t& i);
			void	cleanRequestData(int clientSocket, size_t& i);
			void	addPollFd(int pipeFd);
			int		checkForCgi(Request& req, int& clientSocket);
			bool	isPipeFd(int& fd);
			void	readFromFd(const int& fd);
			bool	checkConnectionUptime(int& clientSocket);
			bool	requestReceived(size_t& totalLength, int& clientSocket);
			void	handleRequest(int& clientSocket);
			void	handleFd(int& clientSocket);
			void	closeConnection(int& clientSocket, size_t& i);
			std::vector<ServerInfo> get_info(void);
	private:
			std::unordered_map<int, t_clientInfo>  	_clientInfos; //key = client socket
			std::vector<ServerInfo>					_info;
			std::vector<struct pollfd> 				_poll_fds;
			std::unordered_map<int, int> 			_connections; // clients and servers
			std::unordered_map<int, int> 			_clientPipe; //key = pipe fd & value = client socket
};
