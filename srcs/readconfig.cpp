#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include "../includes/configExceptions.hpp"

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
	{"root:", "dir-listing:", "allowed-methods:", "upload:", "index:",
		"redirect:", "allowed-cgis:"};
	std::unordered_set<std::string> all_methods =
	{"GET", "POST", "DELETE", "HEAD"};
	std::unordered_set<std::string> all_cgis =
	{".py", ".php"};

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
						if (std::filesystem::is_directory(temploc.root) && value.find("true") != std::string::npos)
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
				case 5:
						temploc.redirection = value;
						break;
				case 6:
						for (std::string cgi : all_cgis)
							if (value.find(cgi) != std::string::npos)
								temploc.cgis.insert(cgi);
						break;
			}
		}
	}
	if (std::filesystem::is_directory(temploc.root) && temploc.name.back() != '/')
		temploc.name += "/";
	server.setnewlocation(temploc);
}

// Recursive function which parses for brackets
int bracketfinder(std::string configfile, std::string type, ServerInfo &server)
{
	std::string 		temp;
	int					brackets = 0;
	std::istringstream	file(configfile);

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
	if (type == "location")
	{
		locations(temp, server);
		std::string newconfig = configfile.substr(configfile.find(temp) + temp.size(), std::string::npos);
		if (newconfig.find("{") != std::string::npos)
		{
			newconfig = newconfig.substr(newconfig.find("location ") + 9, std::string::npos);
			bracketfinder(newconfig, "location", server);
		}
	}
	else if (type == "server")
		config_server(temp, server);
	file.clear();
    file.seekg(0);
	for (std::string line; std::getline(file, line);)
	{
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
      		break;
		temp += line;
	}
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
		{"bodylimit:"},
		{"name:"},
		{"400:"},
		{"403:"},
		{"404:"},
		{"405:"},
		{"500:"},
		{"501:"},
		{"505:"},
		{"415:"},
		{"408:"},
		{"413:"},
		{"414:"},
		{"431:"}

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
						try
						{
							if ((intvalue = std::stoi(value)) < 0)
								throw(wrongTimeout());
						}
						catch (std::exception &e)
						{
							throw(wrongTimeout());
						}
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
				case 5:
						server.setServerName(value);
						break;
				case 6:
						server.setErrorPage(400, value);
						break;
				case 7:
						server.setErrorPage(403, value);
						break;
				case 8:
						server.setErrorPage(404, value);
						break;
				case 9:
						server.setErrorPage(405, value);
						break;
				case 10:
						server.setErrorPage(500, value);
						break;
				case 11:
						server.setErrorPage(501, value);
						break;
				case 12:
						server.setErrorPage(505, value);
						break;
				case 13:
						server.setErrorPage(415, value);
						break;
				case 14:
						server.setErrorPage(408, value);
						break;
				case 15:
						server.setErrorPage(413, value);
						break;
				case 16:
						server.setErrorPage(414, value);
						break;
				case 17:
						server.setErrorPage(431, value);
						break;
				default:
						break;
			}
		}
	}
	if (server.get_ip() == 0 || server.get_port() == 0 || server.getlocationinfo().empty())
		throw(serverNotFilled());
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
		temp = temp.substr(bracketfinder(temp, "server", server), std::string::npos);
		manager.setNewInfo(server);
	}
	return (0);
}
