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
		return (1);
	readconfig(argv[1], manager);
	return (manager.start_servers());
}
