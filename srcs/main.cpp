#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int	readconfig(std::string name, ServerManager &manager);

int	main(int argc, char **argv)
{
	ServerManager	manager;

	if (argc != 2)
	{
		std::cout << "Usage: ./webserv <config file>" << "\n";
		return (1);
	}
	try 
	{
		readconfig(argv[1], manager);
	}
	catch (std::exception &e)
	{
		std::cout << "Wrong arguments in the config! reason: " << e.what() << "\n";
		return (-1);
	}
	return (manager.startServers());
}
