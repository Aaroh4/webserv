#include "../includes/Response.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/Request.hpp"
#include <sys/wait.h>
#include <cstring>
#include <signal.h>

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
	for (const auto& map_content : input._connections)
	{
		this->_connections[map_content.first] = map_content.second;
	}
}

ServerManager ServerManager::operator=(const ServerManager &input)
{
	if (this != &input)
	{
		this->_info = input._info;
		this->_poll_fds = input._poll_fds;
		for (const auto& map_content : input._connections)
		{
			this->_connections[map_content.first] = map_content.second;
		}
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
	try
	{
		int clientSocket = accept(this->_poll_fds[i].fd, nullptr, nullptr);
		if (clientSocket < 0)
			perror("Accept failed");
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
			perror("fcntl(F_SETFL) failed");
		struct pollfd client_pollfd;
		client_pollfd.fd = clientSocket;
		client_pollfd.events = POLLIN | POLLOUT;
		client_pollfd.revents = 0;
		this->_poll_fds.push_back(client_pollfd);
		this->_connections[clientSocket] = i;

		t_clientInfo clientInfo;
		std::memset(&clientInfo, 0, sizeof(t_clientInfo));
		this->_clientInfos[clientSocket] = clientInfo;
		std::cout << "----------------------------------------" << std::endl;
		std::cout << "New client connected on server " << i << "\n";
		std::cout << "Client got clientSocket " << clientSocket << std::endl;
	}
	catch (std::exception& e)
	{
		//500 Error response
	}
}

void	ServerManager::addPipeFd(int pipeFd)
{
	try
	{
		if (fcntl(pipeFd, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
			perror("fcntl(F_SETFL) failed");

		struct pollfd pipe_pollfd;

		pipe_pollfd.fd = pipeFd;
		pipe_pollfd.events = POLLIN;
		pipe_pollfd.revents = 0;
		this->_poll_fds.push_back(pipe_pollfd);
	}
	catch (std::exception& e)
	{
		throw;
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
				return pos + 4; // Position after 0 + \r\n\r\n
		}
		catch(const std::exception& e)
		{
			//500 Error response
		}
		start_pos = pos;
		start_pos += chunk_size + 4; // \r\n before and after chunk
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
		//500 Error response
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
		if (execve(path.c_str(), argv, envp) == -1)
		{
			throw std::runtime_error("execve() failed");
		}
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

int	ServerManager::sendResponse(size_t& i)
{
	int	clientSocket = this->_poll_fds[i].fd;

	if (this->_clientInfos[clientSocket].req->getHost() == this->_info[this->_connections.at(clientSocket)].getServerName()
		|| this->_info[this->_connections.at(clientSocket)].getServerName().empty())
	{
		if (this->_clientInfos[clientSocket].cgiResponseReady == true)
		{
			Response respond(*this->_clientInfos[clientSocket].req);
			respond.setResponseBody(this->_clientInfos[clientSocket].cgiResponse);
			respond.respond(clientSocket, this->_info[this->_connections.at(clientSocket)]);
		}
		else
		{
			Response respond(*this->_clientInfos[clientSocket].req);
			respond.respond(clientSocket, this->_info[this->_connections.at(clientSocket)]);
		}
		removeConnection(clientSocket, i);
	}
	return 0;
}

void ServerManager::removeConnection(int clientSocket, size_t& i)
{
	try
	{
		close(clientSocket);
		std::cout << "removeConnection: ClientSocket " << clientSocket << " closed\n\n" << std::endl;
		this->_poll_fds.erase(this->_poll_fds.begin() + i);
		this->_connections.erase(clientSocket);
		delete this->_clientInfos[clientSocket].req;
		this->_clientInfos.erase(clientSocket);
		i--;
	}
	catch(const std::exception& e)
	{
		//500 Error response
	}
}

void	ServerManager::receiveRequest(size_t& i)
{
	char		buffer[1024] = {0};
	int 		clientSocket = this->_poll_fds[i].fd;
	int 		bytesReceived = 0;
	size_t		totalLength = 0;

	// Receive data until complete request is sent

	try
	{
		if (totalLength == 0 || this->_clientInfos[clientSocket].request.length() < totalLength)
		{
			bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (bytesReceived > 0)
			{
				this->_clientInfos[clientSocket].request.append(buffer, bytesReceived);
				if (totalLength == 0)
					totalLength = getRequestLength(this->_clientInfos[clientSocket].request);
				if (bytesReceived < 1024 && totalLength == 0)
				{
					throw Response::ResponseException400();
				}
			}
			else if (bytesReceived == 0)
			{
				removeConnection(clientSocket, i);
				std::cout << "Client disconnected" << "\n";
			}
			else
			{
				throw Response::ResponseException();
			}
		}
	}
	catch (const Response::ResponseException & e){
		Response obj;
		obj.sendErrorResponse(e.what(), clientSocket, e.responseCode());
		removeConnection(clientSocket, i);
		return ;
	}
	if (totalLength != 0 && totalLength == this->_clientInfos[clientSocket].request.length())
	{
		this->_clientInfos[clientSocket].requestReceived = true;
		try
		{
			Request* request = new Request(this->_clientInfos[clientSocket].request);
			request->parse();
			request->sanitize(this->_info[i]);
			request->printRequest(clientSocket);
			this->_clientInfos[clientSocket].req = request;
			if (checkForCgi(*request, clientSocket) == 1)
			{
				addPipeFd(this->_clientInfos[clientSocket].pipeFd);
			}
		}
		catch (std::exception& e)
		{
			std::cout << "ERROR\n";
		}
	}
}

void	ServerManager::readFromCgiFd(const int& fd)
{
	char buffer[1024];
	ssize_t nbytes;

	nbytes = read(fd, buffer, sizeof(buffer));
	if (nbytes == -1)
	{
		close(fd);
		this->_clientPipe.erase(fd);
		throw std::runtime_error("read() failed");
	}
	else if (nbytes == 0)
	{
		int clientSocket = this->_clientPipe[fd];
		this->_clientInfos[clientSocket].cgiResponseReady = true;
		close(fd);
		this->_clientPipe.erase(fd);
	}
	else
	{
		int clientSocket = this->_clientPipe[fd];
		this->_clientInfos[clientSocket].cgiResponse.append(buffer, nbytes);
	}
}

int	ServerManager::checkForCgi(Request& req, int& clientSocket)
{
	std::string type = req.getType();
	if (type == "cgi/py" || type == "cgi/php")
	{
		std::string contentLen = req.getContentLength();
		std::string method = "REQUEST_METHOD=" + req.getMethod();
		std::string query = "QUERY_STRING=" + req.getQueryString();
		std::string	length;

		if (contentLen.empty())
			length = "CONTENT_LENGTH=" + contentLen;
		else
			length = "CONTENT_LENGTH=0";

		char *envp[] = {
		(char *) method.c_str(),
		(char *) query.c_str(),
		(char *) length.c_str(),
		nullptr
		};

		std::string location = std::filesystem::canonical("/proc/self/exe");
		size_t lastDash = location.find_last_of("/");
		location.erase(lastDash + 1, location.length() - (lastDash + 1));
		location += "www" + req.getUrl();
		std::cout << "location: " << location << std::endl;
		try
		{
			runCgi(location, envp, clientSocket);
			return 1;
		}
		catch (std::exception& e)
		{
			throw;
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
		int pollcount = poll(this->_poll_fds.data(), this->_poll_fds.size(), 0);

		if (pollcount < 0)
		{
			if (errno != EINTR)
				std::cerr << "Poll failed likely due to IP or Port being wrong!" << std::endl;
			break ;
		}
		for (size_t i = 0; i < this->_poll_fds.size(); i++)
		{
			if (this->_poll_fds[i].revents & POLLIN)
			{
				if (i < this->get_info().size())
					addNewConnection(i);
				else if (isPipeFd(this->_poll_fds[i].fd))
					readFromCgiFd(this->_poll_fds[i].fd);
				else
					receiveRequest(i);
			}
			if (this->_poll_fds[i].revents & POLLOUT && this->_clientInfos[this->_poll_fds[i].fd].requestReceived == true)
				sendResponse(i);
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
