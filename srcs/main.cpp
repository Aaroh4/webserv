#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/configExceptions.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <filesystem>
#include <signal.h>

int	readconfig(std::string name, ServerManager &manager);

void	sigint_handler(int signum)
{
	(void)signum;
}

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
	try
	{
		readconfig(path, manager);
		if (manager.get_info().empty())
			throw(noServers());
	}
	catch (std::exception &e)
	{
		std::cout << "Wrong arguments in the config! reason: " << e.what() << "\n";
		return (-1);
	}		
	signal(SIGINT, sigint_handler);
	manager.startServers();
	return (0);
}
