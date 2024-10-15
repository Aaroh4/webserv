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
	fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	struct pollfd client_pollfd;
	client_pollfd.fd = clientSocket;
	client_pollfd.events = POLLIN;
	this->_poll_fds.push_back(client_pollfd);
	this->_connections[clientSocket] = i;
	std::cout << "New client connected on server " << i << "\n";
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
			return 0;
		}
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
	
	Response respond(request);
	respond.respond(clientSocket, this->_info[this->_connections.at(clientSocket)]);
}

void ServerManager::removeConnection(int clientSocket, size_t& i) 
{
	close(clientSocket);
	this->_poll_fds.erase(this->_poll_fds.begin() + i);
	this->_connections.erase(clientSocket);
	i--;
}

void	ServerManager::receiveRequest(size_t& i)
{
	char		buffer[1024] = {0};
	int 		clientSocket = this->_poll_fds[i].fd;
	std::string	http_request;
	int 		bytes_received = 0;
	size_t		total_length = 0;

	// Receive data until complete request is sent
	try 
	{
		while (total_length == 0 || http_request.length() < total_length)
		{
		bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytes_received > 0)
		{
			http_request.append(buffer, bytes_received);
			if (total_length == 0)
				total_length = getRequestLength(http_request);
			if (bytes_received < 1024 && total_length == 0)
				std::cout << "bad Request\n";
		}
		else if (bytes_received == 0)
		{
			std::cout << "Client disconnected" << "\n";
			break;
		}
		else
		{
			std::cout << "bytes received: " << bytes_received << "\n";
			perror("Recv error");
			break;
		}
	}
	}
	catch (std::exception& e)
	{

	}
	
	if (total_length != http_request.length())
	{
		std::cout << "bad request\n";
		removeConnection(clientSocket, i);
	}
	handleRequest(http_request, clientSocket);
	removeConnection(clientSocket, i);
}

void	ServerManager::runServers()
{
	while (true)
	{
		int pollcount = poll(this->_poll_fds.data(), this->_poll_fds.size(), 100);

		if (pollcount < 0)
			break ;
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
		serverAddress.sin_port = htons(this->get_info()[i].get_port(0));
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
		temp_s_pollfd.events = POLLIN;
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
