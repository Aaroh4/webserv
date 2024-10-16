#include "../includes/Response.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/Request.hpp"

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
}

void	ServerManager::addNewConnection(size_t i)
{
	int clientSocket = accept(this->_poll_fds[i].fd, nullptr, nullptr);
	if (clientSocket < 0)
		perror("Accept failed");
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		perror("fcntl(F_SETFL) failed");
	struct pollfd client_pollfd;
	client_pollfd.fd = clientSocket;
	client_pollfd.events = POLLIN;
	this->_poll_fds.push_back(client_pollfd);
	this->_connections[clientSocket] = i;
	this->_clients[clientSocket] = "";
	std::cout << "New client connected on server " << i << "\n";
	std::cout << "Client got clientSocket " << clientSocket << std::endl;
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
			std::cout << e.what() << std::endl;
			return 0;
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
	size_t	total_length = 0;
	size_t	start = request.find("Content-Length: ");

	try
	{
		if (request.substr(0, 4) == "GET ")
			return headers_length;
		else if (start != std::string::npos)
		{
			start += 16;
			end = request.find("\r\n", start);
			content_length = std::stoi(request.substr(start, end - start));
			total_length = content_length + headers_length;
		}
		else if (request.find("Transfer-Encoding: chunked") != std::string::npos)
			total_length = findLastChunk(request, headers_length);
		else
			return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return total_length;
}

void	ServerManager::handleRequest(std::string& http_request, int clientSocket)
{
	Request request(http_request);
	request.parse();
	request.sanitize();
	if (request.getHost() == this->_info[this->_connections.at(clientSocket)].getServerName())
	{
		Response respond(request);
		respond.respond(clientSocket, this->_info[this->_connections.at(clientSocket)]);
	}	
}

void ServerManager::removeConnection(int clientSocket, size_t& i)
{
	close(clientSocket);
	std::cout << "ClientSocket " << clientSocket << " closed\n\n" << std::endl;
	this->_poll_fds.erase(this->_poll_fds.begin() + i);
	this->_connections.erase(clientSocket);
	this->_clients.erase(clientSocket);
	i--;
}

void	ServerManager::receiveRequest(size_t& i)
{
	char		buffer[1024] = {0};
	int 		clientSocket = this->_poll_fds[i].fd;
	int 		bytes_received = 0;
	size_t		total_length = 0;

	// Receive data until complete request is sent
	try
	{
		if (total_length == 0 || this->_clients[clientSocket].length() < total_length)
		{
			bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
			if (bytes_received > 0)
			{
				this->_clients[clientSocket].append(buffer, bytes_received);
				if (total_length == 0)
					total_length = getRequestLength(this->_clients[clientSocket]);
				if (bytes_received < 1024 && total_length == 0)
					throw Response::ResponseException400();
			}
			else if (bytes_received == 0)
			{
				std::cout << "Client disconnected" << "\n";
			}
			else
			{
				std::cout << "recv failed" << "\n";
				perror("error: ");
				throw Response::ResponseException();
			}
		}
	}
	catch (const Response::ResponseException & e){
		Response obj;
		obj.sendErrorResponse(e.what(), clientSocket, e.responseCode());
		removeConnection(clientSocket, i);
	}
/*	try {
		if (total_length != http_request.length())
			throw Response::ResponseException400();
	} catch (const Response::ResponseException & e){
		Response obj;
		obj.sendErrorResponse(e.what(), clientSocket, e.responseCode());
		removeConnection(clientSocket, i);
		return ;
	}*/
	if (total_length == this->_clients[clientSocket].length())
	{
		handleRequest(this->_clients[clientSocket], clientSocket);
		removeConnection(clientSocket, i);
	}
}

void	ServerManager::runServers()
{
	while (true)
	{
		int pollcount = poll(this->_poll_fds.data(), this->_poll_fds.size(), 1000);

		if (pollcount < 0)
		{
			std::cerr << "poll failed" << std::endl;
			break ;
		}
		for (size_t i = 0; i < this->_poll_fds.size(); i++)
		{
			if (this->_poll_fds[i].revents & POLLIN) 
			{
				if (i < this->get_info().size())
					addNewConnection(i);
				else
					receiveRequest(i);
			}
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

		setsockopt(this->get_info()[i].getsocketfd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if (bind(this->get_info()[i].getsocketfd(), (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
		{
			std::cout << "Bind failed!" << "\n";
			return (1);
		}
		fcntl(this->get_info()[i].getsocketfd(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		listen(this->get_info()[i].getsocketfd(), 5); // ERROR CHECK HERE TOO LAZY

		struct pollfd temp_s_pollfd;
		temp_s_pollfd.fd = this->get_info()[i].getsocketfd();
		temp_s_pollfd.events = POLLIN | POLLOUT;
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
