#include "../includes/Response.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/Request.hpp"
#include <sys/wait.h>
#include <cstring>

bool sigint_sent = false;

void	sigint_handler(int signum)
{
	(void)signum;
	sigint_sent = true;
}

ServerManager::ServerManager()
{}

ServerManager::ServerManager(const ServerManager &input)
{
	this->_info = input._info;
	this->_poll_fds = input._poll_fds;
	this->_connections = input._connections;
	this->_clientPipe = input._clientPipe;
	this->_clientInfos = input._clientInfos;
}

ServerManager ServerManager::operator=(const ServerManager &input)
{
	if (this != &input)
	{
		this->_info = input._info;
		this->_poll_fds = input._poll_fds;
		this->_connections = input._connections;
		this->_clientPipe = input._clientPipe;
		this->_clientInfos = input._clientInfos;
	}
	return (input);
}

ServerManager::~ServerManager()
{
	for (auto& it: this->_clientInfos)
	{
		close(it.first);
		close(it.second.pipeFd);
		delete it.second.req;
	}
}

void	ServerManager::addNewConnection(size_t& i)
{
	int clientSocket;
	try
	{
		clientSocket = accept(this->_poll_fds[i].fd, nullptr, nullptr);
		if (clientSocket < 0)
			throw Response::ResponseException();
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
			throw Response::ResponseException();
		struct pollfd client_pollfd;
		client_pollfd.fd = clientSocket;
		client_pollfd.events = POLLIN | POLLOUT;
		client_pollfd.revents = 0;
		this->_poll_fds.push_back(client_pollfd);
		this->_connections[clientSocket] = i;

		t_clientInfo clientInfo = {};
		this->_clientInfos[clientSocket] = clientInfo;
		std::cout << "----------------------------------------" << std::endl;
		std::cout << "New client connected on server " << i << "\n";
		std::cout << "Client got clientSocket " << clientSocket << std::endl;
	}
	catch (Response::ResponseException &e)
	{
		std::cerr << e.what() << " in addNewConnection" << std::endl;
		this->_clientInfos[clientSocket].responseStatus = 500;
		this->_clientInfos[clientSocket].requestReceived = true;
		throw ;
	}
}

void	ServerManager::addPollFd(int pipeFd)
{
	try
	{
 		if (fcntl(pipeFd, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
			throw Response::ResponseException();

		struct pollfd pipe_pollfd;

		pipe_pollfd.fd = pipeFd;
		pipe_pollfd.events = POLLIN;
		pipe_pollfd.revents = 0;
		this->_poll_fds.push_back(pipe_pollfd);
	}
	catch (Response::ResponseException &e)
	{
		std::cerr << e.what() << " in addPipeFd"<< std::endl;
		throw ;
	}
}

size_t	ServerManager::findLastChunk(std::string& request, size_t start_pos)
{
	int	chunk_size = 1;
	size_t pos = 0;

	while (chunk_size != 0)
	{
		pos = request.find("\r\n", start_pos);
		try
		{
			chunk_size = std::stoi(request.substr(start_pos, pos - start_pos), nullptr, 16);
			if (chunk_size == 0)
				return pos + 4;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << " stoi failed in findLastChunk" << std::endl;
			throw Response::ResponseException();
		}
		start_pos = pos;
		start_pos += chunk_size + 4;
	}
	return pos + 4;
}

size_t	ServerManager::getRequestLength(std::string& request)
{
	size_t	end = request.find("\r\n\r\n");
	if (end == std::string::npos)
		return 0;

	size_t	headers_length = end + 4;
	size_t	content_length = 0;
	size_t	totalLength = 0;
	size_t	start = request.find("Content-Length: ");

	try
	{
		if (request.substr(0, 4) == "GET " || request.substr(0, 7) == "DELETE " )
			return headers_length;
		else if (start != std::string::npos)
		{
			start += 16;
			end = request.find("\r\n", start);
			content_length = std::stoi(request.substr(start, end - start));
			totalLength = content_length + headers_length;
		}
		else if (request.find("Transfer-Encoding: chunked") != std::string::npos)
			totalLength = findLastChunk(request, headers_length);
		else
			return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << " in getRequestLength" << std::endl;
		throw Response::ResponseException();
	}
	return totalLength;
}

void	ServerManager::runCgi(std::string path, char** envp, int& clientSocket)
{
	int	pipeFd[2];

	if (pipe(pipeFd) == -1)
	{
		throw std::runtime_error("pipe() failed");
	}
	int	pid = fork();
	if (pid == -1)
	{
		throw std::runtime_error("fork() failed");
	}
	if (pid == 0)
	{
		close(pipeFd[0]);
		if (dup2(pipeFd[1], STDOUT_FILENO) == -1)
		{
			throw std::runtime_error("dup2() failed");
		}
		close(pipeFd[1]);
		char *argv[] = { (char*)path.c_str(), nullptr };
		execve(path.c_str(), argv, envp);
		std::terminate();
	}
	else
	{
		close(pipeFd[1]);
		int	status;
		if  (waitpid(pid, &status, 0) == -1)
		{
			throw std::runtime_error("waitpid() failed");
		}
		this->_clientInfos[clientSocket].pipeFd = pipeFd[0];
		this->_clientPipe[pipeFd[0]] = clientSocket;
	}
}

void	ServerManager::sendResponse(size_t& i)
{
	int	clientSocket = this->_poll_fds[i].fd;
	int	pipeFd = this->_clientInfos[clientSocket].pipeFd;

	if (this->_clientInfos[clientSocket].responseStatus != 0 && this->_clientInfos[clientSocket].ResponseReady == true)
	{
		Response::sendErrorPage(this->_clientInfos[clientSocket].responseStatus, clientSocket, this->_clientInfos[clientSocket].ResponseBody, this->_clientInfos[clientSocket].req->getCookie());
		cleanRequestData(clientSocket, i);
		return ;
	}

	if (!pipeFd)
		pipeFd = this->_clientInfos[clientSocket].req->getFileFD();
	if (this->_clientInfos[clientSocket].req->getHost() == this->_info[this->_connections.at(clientSocket)].getServerName()
		|| this->_info[this->_connections.at(clientSocket)].getServerName().empty())
	{
		if (this->_clientPipe.find(pipeFd) != this->_clientPipe.end() && this->_clientInfos[clientSocket].ResponseReady == false)
		{
			return;
		}
		else if (this->_clientInfos[clientSocket].ResponseReady == true)
		{
			Response respond(*this->_clientInfos[clientSocket].req);
			respond.setResponseBody(this->_clientInfos[clientSocket].ResponseBody);
			respond.respond(clientSocket, this->_info[this->_connections.at(clientSocket)]);
			this->_clientInfos[clientSocket].req->setSanitizeStatus(respond.getSanitizeStatus());
		}
		else
		{
			Response respond(*this->_clientInfos[clientSocket].req);
			respond.respond(clientSocket, this->_info[this->_connections.at(clientSocket)]);
			this->_clientInfos[clientSocket].req->setSanitizeStatus(respond.getSanitizeStatus());
		}
		cleanRequestData(clientSocket, i);
	}
}

void ServerManager::cleanRequestData(int clientSocket, size_t& i)
{
	try
	{
		std::string connectionStatus;
		int pipeFd = this->_clientInfos[clientSocket].pipeFd;

		if (!pipeFd && this->_clientInfos[clientSocket].failedToReceiveRequest == false)
			pipeFd = this->_clientInfos[clientSocket].req->getFileFD();
		if (pipeFd){
			for (auto it = this->_poll_fds.begin(); it != this->_poll_fds.end();){
				if (it->fd == pipeFd)
					it = this->_poll_fds.erase(it);
				else
					++it;
			}
			close(pipeFd);
		}
		this->_clientPipe.erase(pipeFd);
		this->_clientInfos[clientSocket].request = "";
		this->_clientInfos[clientSocket].requestReceived = false;
		this->_clientInfos[clientSocket].ResponseReady = false;
		this->_clientInfos[clientSocket].ResponseBody = "";
		this->_clientInfos[clientSocket].requestLength = 0;

		if (this->_clientInfos[clientSocket].failedToReceiveRequest == true)
			connectionStatus = "close";
		else
			connectionStatus = this->_clientInfos[clientSocket].req->getConnectionHeader();
		if (connectionStatus == "close" || checkConnectionUptime(clientSocket) == true
			|| this->_clientInfos[clientSocket].responseStatus != 0  || this->_clientInfos[clientSocket].req->getSanitizeStatus() != 200)
			closeConnection(clientSocket, i);
		this->_clientInfos[clientSocket].failedToReceiveRequest = false;
	} catch(const std::exception& e) {
		std::cout << e.what() <<" in request cleanup" << std::endl;
	}
}

void	ServerManager::closeConnection(int& clientSocket, size_t& i)
{
	close(clientSocket);
	this->_poll_fds.erase(this->_poll_fds.begin() + i);
	i--;
	this->_connections.erase(clientSocket);
	if (this->_clientInfos[clientSocket].failedToReceiveRequest == false && this->_clientInfos[clientSocket].req != nullptr)
	{
		delete this->_clientInfos[clientSocket].req;
		this->_clientInfos[clientSocket].req = nullptr;
	}
	this->_clientInfos.erase(clientSocket);
	std::cout << "ClientSocket " << clientSocket << " disconnected\n\n" << std::endl;
}

bool	ServerManager::checkConnectionUptime(int& clientSocket)
{
	try {
		if (this->_clientInfos[clientSocket].latestRequest != 0)
		{
			std::time_t currentTime = std::time(nullptr);
			unsigned int diff = currentTime - this->_clientInfos[clientSocket].latestRequest;
			if (diff >= this->_info[this->_connections[clientSocket]].get_timeout())
				return true;
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << "in ConnectionUpTime" << std::endl;
	}
	return false;
}

bool	ServerManager::requestReceived(size_t& totalLength, int& clientSocket)
{
	if (totalLength != 0 && totalLength == this->_clientInfos[clientSocket].request.length())
	{
		this->_clientInfos[clientSocket].requestReceived = true;
		return true;
	}
	return false;
}

void	ServerManager::handleFd(int& clientSocket)
{
	if (this->_clientInfos[clientSocket].req->getFileFD() > 0)
	{
		this->_clientPipe[this->_clientInfos[clientSocket].req->getFileFD()] = clientSocket;
		addPollFd(this->_clientInfos[clientSocket].req->getFileFD());
	}
}

void	ServerManager::handleRequest(int& clientSocket)
{
	try
	{
		if (this->_clientInfos[clientSocket].req != nullptr)
		{
			delete this->_clientInfos[clientSocket].req;
			this->_clientInfos[clientSocket].req = nullptr;
		}
		this->_clientInfos[clientSocket].req = new Request(this->_clientInfos[clientSocket].request, this->_info[this->_connections[clientSocket]].getBodylimit());
		this->_clientInfos[clientSocket].req->parse();
		this->_clientInfos[clientSocket].req->sanitize(this->_info[this->_connections[clientSocket]]);
		if (this->_clientInfos[clientSocket].req->getConnectionHeader() == "keep-alive")
			this->_clientInfos[clientSocket].latestRequest = std::time(nullptr);

		std::cout << "request " << this->_clientInfos[clientSocket].request << std::endl;

		if (checkForCgi(*this->_clientInfos[clientSocket].req, clientSocket) == 1)
			addPollFd(this->_clientInfos[clientSocket].pipeFd);
		else
		{
			this->_clientInfos[clientSocket].req->openFile(this->_info[this->_connections[clientSocket]]);
			handleFd(clientSocket);
		}
	} catch (Response::ResponseException &e){
		std::cerr << e.what()<< " in receiveRequest" << std::endl;
		this->_clientInfos[clientSocket].responseStatus = e.responseCode();
		this->_clientInfos[clientSocket].req->openErrorFile(this->_info[this->_connections[clientSocket]], e.responseCode());
		handleFd(clientSocket);
		throw;
	} catch (std::exception &e){
		if (this->_clientInfos[clientSocket].responseStatus == 200)
			this->_clientInfos[clientSocket].responseStatus = 400;
		this->_clientInfos[clientSocket].requestReceived = true;
		throw;
	}
}

void	ServerManager::receiveRequest(size_t& i)
{
	char		buffer[1024] = {0};
	int 		clientSocket = this->_poll_fds[i].fd;
	int 		bytesReceived = 0;
	size_t		totalLength = this->_clientInfos[clientSocket].requestLength;
	// Receive data until complete request is received
	if (this->_clientInfos[clientSocket].requestReceived == true)
		return;
	try
	{
		if (totalLength == 0 || this->_clientInfos[clientSocket].request.length() < totalLength)
		{
			bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (bytesReceived > 0)
			{
				this->_clientInfos[clientSocket].request.append(buffer, bytesReceived);
				if (totalLength == 0)
				{
					this->_clientInfos[clientSocket].requestLength = getRequestLength(this->_clientInfos[clientSocket].request);
					totalLength = this->_clientInfos[clientSocket].requestLength;
				}
				if ((bytesReceived < 1024 && totalLength == 0)
					|| (bytesReceived < 1024 && this->_clientInfos[clientSocket].request.length() < totalLength))
				{
					this->_clientInfos[clientSocket].failedToReceiveRequest = true;
					throw Response::ResponseException400();
				}
			}
			else if (checkConnectionUptime(clientSocket) == true && bytesReceived == 0)
			{
				closeConnection(clientSocket, i);
			}
			else if (bytesReceived == -1)
				return ;
		}
		if (this->_clientInfos[clientSocket].request.length() > totalLength)
		{
			this->_clientInfos[clientSocket].failedToReceiveRequest = true;
			throw Response::ResponseException400();
		}
	} catch (Response::ResponseException &e){
		std::cerr << e.what() << " in first part of receiveRequest"<< std::endl;
		this->_clientInfos[clientSocket].responseStatus = e.responseCode();
		this->_clientInfos[clientSocket].ResponseReady = true;
		this->_clientInfos[clientSocket].requestReceived = true;
		throw;
	} catch (std::exception &e){
		throw;
	}
	if (requestReceived(totalLength, clientSocket) == true)
	{
		try
		{
			handleRequest(clientSocket);
		} catch (Response::ResponseException &e){
			std::cerr << e.what() << " in receiveRequest"<< std::endl;
			throw;
		} catch (std::exception &e){
			throw;
		}
	}
}

void	ServerManager::readFromFd(const int& fd)
{
	char buffer[8192];
	ssize_t nbytes;

	nbytes = read(fd, buffer, sizeof(buffer));
	if (nbytes == -1)
	{
		close(fd);
		this->_clientPipe.erase(fd);
		throw std::runtime_error("read() failed");
	}
	else if (nbytes == 0 || nbytes < 8192)
	{
		close(fd);
		int clientSocket = this->_clientPipe[fd];
		this->_clientInfos[clientSocket].ResponseBody.append(buffer, nbytes);
		this->_clientInfos[clientSocket].ResponseReady = true;
		this->_clientPipe.erase(fd);
	}
	else
	{
		int clientSocket = this->_clientPipe[fd];
		this->_clientInfos[clientSocket].ResponseBody.append(buffer, nbytes);
	}
}

int	ServerManager::checkForCgi(Request& req, int& clientSocket)
{
	std::string type = req.getType();
	if (type == "cgi/py" || type == "cgi/php")
	{
		std::string contentLen = req.getContentLength();
		std::string method = "REQUEST_METHOD=" + req.getMethod();
		std::string query;
		std::string	length;
		std::string timeOut = "TIMEOUT=";

		std::string temp = std::to_string(this->_info[this->_connections.at(clientSocket)].get_timeout());
		if (temp.empty())
			timeOut += DEFAULT_TIMEOUT;
		else
			timeOut += temp;
			
		if (contentLen.empty())
			length = "CONTENT_LENGTH=" + contentLen;
		else
			length = "CONTENT_LENGTH=0";
		if (!req.getQueryString().empty())
			query = "QUERY_STRING=" + req.getQueryString();
		else
			query = "QUERY_STRING=" + req.getBody();

		char *envp[] = {
		(char *) method.c_str(),
		(char *) query.c_str(),
		(char *) length.c_str(),
		(char *) timeOut.c_str(),
		nullptr
		};

		std::string location = std::filesystem::canonical("/proc/self/exe");
		size_t lastDash = location.find_last_of("/");
		location.erase(lastDash + 1, location.length() - (lastDash + 1));
		location += req.getRoot() + "/" + req.getUrl().substr(req.getOrigLocLen().length(), std::string::npos);
		try
		{
			runCgi(location, envp, clientSocket);
			return 1;
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << " in checkForCgi" << std::endl;
			throw Response::ResponseException();
		}
	}
	else
		return 0;
}

bool	ServerManager::isPipeFd(int& fd)
{
	if (this->_clientPipe[fd] != 0)
	{
		return true;
	}
	return false;
}

void	ServerManager::runServers()
{
	signal(SIGINT, sigint_handler);
	while (sigint_sent == false)
	{
		try{
			int pollcount = poll(this->_poll_fds.data(), this->_poll_fds.size(), 5000);

			if (pollcount < 0)
			{
				if (errno != EINTR)
					std::cerr << "Poll failed likely due to IP or Port being wrong!" << std::endl;
				break ;
			}
			for (size_t i = 0; i < this->_poll_fds.size(); i++)
			{
				bool pipeFd = false;
				if (this->_poll_fds[i].revents & POLLIN)
				{
					if (i < this->get_info().size())
						addNewConnection(i);
					else if (isPipeFd(this->_poll_fds[i].fd))
					{
						readFromFd(this->_poll_fds[i].fd);
						pipeFd = true;
					}
					else
						receiveRequest(i);
				}
				if (this->_poll_fds[i].revents & POLLOUT && this->_clientInfos[this->_poll_fds[i].fd].requestReceived == true
					&& pipeFd == false)
					sendResponse(i);
			}
		} catch (std::exception &e){
			std::cerr << "Mainloop catched an error " << e.what() << std::endl;
		}
	}
}

int	ServerManager::startServers()
{
	for (size_t i = 0; i < this->get_info().size(); i++)
	{
		sockaddr_in serverAddress;
		int			opt = 1;

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(this->get_info()[i].get_port());
		serverAddress.sin_addr.s_addr = htonl(this->get_info()[i].get_ip());

		if (setsockopt(this->get_info()[i].getsocketfd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			std::cout << "setsockopt() failed!" << "\n";
			return (1);
		}
		if (bind(this->get_info()[i].getsocketfd(), (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
		{
			std::cout << "bind() failed!" << "\n";
			return (1);
		}
		if (fcntl(this->get_info()[i].getsocketfd(), F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
		{
			std::cout << "fcntl() failed!" << "\n";
			return (1);
		}
		if (listen(this->get_info()[i].getsocketfd(), 5) == -1)
		{
			std::cout << "listen() failed!" << "\n";
			return (1);
		}
		struct pollfd temp_s_pollfd;
		temp_s_pollfd.fd = this->get_info()[i].getsocketfd();
		temp_s_pollfd.events = POLLIN | POLLOUT;
		temp_s_pollfd.revents = 0;
		this->_poll_fds.push_back(temp_s_pollfd);
	}
	runServers();
	return (0);
}

void ServerManager::setNewInfo(ServerInfo server)
{
	this->_info.push_back(server);
}

std::vector<ServerInfo> ServerManager::get_info()
{
	return (this->_info);
}
