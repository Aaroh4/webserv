#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/configExceptions.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <filesystem>
#include <signal.h>

int	readconfig(std::string name, ServerManager &manager);

int	main(int argc, char **argv)
{
	std::string path;
	ServerManager	manager;

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
	try
	{
		readconfig(path, manager);
		if (manager.get_info().empty())
			throw(noServers());
	}
	catch (std::exception &e)
	{
		std::cerr << "Wrong arguments in the config! reason: " << e.what() << "\n";
		return (-1);
	}		
	manager.startServers();
	return (0);
}
