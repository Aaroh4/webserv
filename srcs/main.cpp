#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <filesystem>

int	readconfig(std::string name, ServerManager &manager);

int	main(int argc, char **argv)
{
	ServerManager	manager;
	std::string path;

	if (argc != 2)
		path = DEFAULT; //defined in ServerManager.hpp
	else
		path = argv[1];
	std::cout << "Welcome to use our webServ! Running server config: " << path << std::endl;
	std::filesystem::path file = path;
	if (!std::filesystem::exists(file) ||
	!std::filesystem::is_regular_file(file)){
		std::cout << "invalid path" << std::endl;
		return (1);
	}
	readconfig(path, manager);
	return (manager.startServers());
}
