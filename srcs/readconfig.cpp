#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/configExceptions.hpp"
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <filesystem>

ServerInfo	config_server(std::string temp, ServerInfo &server);
int bracketfinder(std::string configfile, std::string type, ServerInfo &server);

// Function to make a string lowercase
std::string toLowerCase(const std::string str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

// Location parsing
void locations(std::string temp, ServerInfo &server)
{
	std::vector<std::string> location_configs =
	{"root:", "dir-listing:", "allowed-methods:", "upload:", "index:"};
	std::unordered_set<std::string> all_methods =
	{"GET", "POST", "DELETE", "HEAD"};

	location temploc;
	temploc.name = temp.substr(0, temp.find(" "));
	for (size_t i = 0; i < location_configs.size(); i++) // Loop that checks out which configs are added
	{
		size_t pos = toLowerCase(temp).find(location_configs.at(i));
		if (pos != std::string::npos)
		{
			std::string value;
			value = cutFromTo(temp, pos + location_configs.at(i).size(), "\n");
			value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
			switch (i)
			{
				case 0:
						temploc.root = value;
						break;
				case 1:
						if (std::filesystem::is_directory(temploc.root + temploc.name) && value.find("true") != std::string::npos)
							temploc.dirList = true;
						else
							temploc.dirList = false;
											break;
				case 2:
						for (std::string method : all_methods)
							if (value.find(method) != std::string::npos)
								temploc.methods.insert(method);
						break;
				case 3:
						temploc.upload = value;
						break;
				case 4:
						temploc.index = value;
						break;
			}
		}
	}
	if (std::filesystem::is_directory(temploc.root + temploc.name) && temploc.name.back() != '/')
		temploc.name += "/";
	//std::cout << "name: " << temploc.name << std::endl;
	server.setnewlocation(temploc);
}

// Recursive function which parses for brackets
int bracketfinder(std::string configfile, std::string type, ServerInfo &server)
{
	std::string 		temp;
	int					brackets = 0;
	std::istringstream	file(configfile);

	//std::cout << "type" << type << std::endl;
	for (std::string line; std::getline(file, line);)
	{
		if (line.find("{") != std::string::npos)
		{
			temp += line + "\n";
			brackets++;
			break;
		}
	}
	for (std::string line; std::getline(file, line) && brackets > 0;)
	{
		temp += line + "\n";
		if (line.find("{") != std::string::npos)
			brackets++;
		if (line.find("}") != std::string::npos && brackets > 0)
			brackets--;
	}
	//std::cout << "\n\n temp: " << temp  << " type : " << type << "\n\n" << std::endl;
	if (type == "location")
	{
		locations(temp, server);
		std::string newconfig = configfile.substr(configfile.find(temp) + temp.size(), std::string::npos);
		if (newconfig.find("{") != std::string::npos)
		{
			//std::cout << "asd" << std::endl;
			newconfig = newconfig.substr(newconfig.find("location ") + 9, std::string::npos);
			//std::cout << "asd2" << std::endl;
			bracketfinder(newconfig, "location", server);
		}
	}
	else if (type == "server")
		config_server(temp, server);
	return (temp.size());
}

// Recursive function for parsing
ServerInfo	config_server(std::string temp, ServerInfo &server)
{
	std::vector<std::string> server_configs
	{
		{"host:"},
		{"port:"},
		{"location "},
		{"timeout:"},
		{"bodylimit:"}

	};

	server.setsocketfd(socket(AF_INET, SOCK_STREAM, 0));
	for (size_t j = 0; j < server_configs.size(); j++) // Loop that checks out which configs are added
	{
		size_t pos = toLowerCase(temp).find(server_configs.at(j));
		if (pos != std::string::npos)
		{
			std::string	value;
			int intvalue;
			value = cutFromTo(temp.substr(pos + server_configs.at(j).size()), 0, "\n");
			if (j != 2)
				value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
			switch (j)
			{
				case 0:
					server.set_ip(value);
					break;
				case 1:
					try
					{
						if ((intvalue = std::stoi(value)) >= 0)
							server.setnew_port(intvalue);
					}
					catch (std::exception &e)
					{
						throw(wrongPort());
					}
					break;
				case 2:
				 	if (value.find("{") != std::string::npos)
				 		bracketfinder(temp.substr(temp.find(value), std::string::npos), "location", server);
					else
						std::cout << "No opening bracket on the same line as location!" << "\n";
					break;
				case 3:
						server.setTimeout(value);
					break;
				case 4:
					try
					{
						if ((intvalue = std::stoi(value)) >= 0)
							server.setBodylimit(intvalue);
					}
					catch (std::exception &e)
					{
						throw(wrongBodyLimit());
					}
					break;
				default:
					break;
			}
		}
	}
	return (server);
}

// start of the config reading
int	readconfig(std::string name, ServerManager &manager)
{
	std::ifstream	configfile(name);
	if (!configfile.is_open())
	{
		throw (noPermissions());
	}
	std::string		temp;
	int				i = 0;

	for (std::string line; std::getline(configfile, line);)
	{
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
			i++;
		if (i > 0)
			temp += line + "\n";
	}
	for (int info = 1; info <= i; info++)
	{
		ServerInfo server;
		//std::cout << temp << std::endl;
		temp = temp.substr(bracketfinder(temp, "server", server), std::string::npos);
		//if (temp.find("{") != std::string::npos)
		//	temp = temp.substr(temp.find("{"), std::string::npos);
		manager.setNewInfo(server);
	}
	return (0);
}
