#include "../includes/ServerInfo.hpp"
#include "../includes/ServerManager.hpp"
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <filesystem>

ServerInfo	config_server(std::string temp, ServerInfo &server);
int brackets(std::string configfile, std::string type, ServerInfo &server);

std::string toLowerCase(const std::string str) 
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

inline std::string cutFromTo(std::string input, int start, std::string last)
{
	input = input.substr(start, std::string::npos);
	return (input.substr(0, input.find(last)));
}

int brackets(std::string configfile, std::string type, ServerInfo &server)
{
	std::string 		temp;
	int					brackets = 1;
	std::istringstream	file(configfile);

	for (std::string line; std::getline(file, line) && brackets != 0;)
	{
		temp += line + "\n";
		if (line.find("{") != std::string::npos)
			brackets++;
		if (line.find("}") != std::string::npos && brackets > 0)
			brackets--;
	}
	if (type == "location")
	{
		std::vector<std::string> string_to_case
		{
			{"GET"},
			{"POST"},
			{"DELETE"},
			{"HEAD"}
		};

		location temploc;		
		temploc.name = temp.substr(0, temp.find(" "));
		temploc.root = cutFromTo(temp, temp.find("root: ") + 6, "\n");
		std::string temp1;
		if (temp.find("allowedmethods: ") != std::string::npos)
			temp1 = cutFromTo(temp, temp.find("allowedmethods: "), "\n");
		//std::cout << temp1 << std::endl;
		for (size_t i = 0; i < string_to_case.size(); i++)
		{
			switch (temp1.find(string_to_case.at(i)))
			{
				case 0:
					temploc.methods.insert("END");
					break;
				case 1:
					temploc.methods.insert("POST");
					break;
				case 2:
					temploc.methods.insert("DELETE");
					break;
				case 3:
					temploc.methods.insert("HEAD");
					break;
			}
		}
		//std::cout << "root: " << temploc.root << "\n";
		if (std::filesystem::is_directory(temploc.root + temploc.name) && cutFromTo(temp, temp.find("dir-listing: "), "\n").find("true") != std::string::npos)														// THIS IS FOR TESTING REMEMBER TO SWITCH OUT
			temploc.dirList = true; 					// THIS IS FOR TESTING REMEMBER TO SWITCH OUT
		else											// THIS IS FOR TESTING REMEMBER TO SWITCH OUT
			temploc.dirList = false; 					// THIS IS FOR TESTING REMEMBER TO SWITCH OUT
		//std:: cout << "name : " << temploc.name << temploc.dirList << std::endl;
		server.setnewlocation(temploc);
		config_server(temp.substr(temp.find("{"), temp.size()), server);
	}
	else if (type == "server")
		config_server(temp, server);
	return (temp.size());
}

ServerInfo	config_server(std::string temp, ServerInfo &server)
{
	std::vector<std::string> string_to_case
	{
		{"host: "},
		{"host:"},
		{"port: "},
		{"port:"},
		{"location "},
	};

	server.setsocketfd(socket(AF_INET, SOCK_STREAM, 0));
	for (size_t j = 0; j < string_to_case.size(); j++)
	{
		size_t pos = toLowerCase(temp).find(string_to_case.at(j));
		if (pos != std::string::npos)
		{
			std::string	value;
			std::istringstream stream(temp.substr(pos + string_to_case.at(j).size(), std::string::npos));
			std::getline(stream, value, '\n');
			switch (j)
			{
				case 0:
				case 1:
					server.set_ip(value);
					break;
				case 2:
				case 3:
					server.setnew_port(std::stoi(value));
					break;
				 case 4:
				 	if (value.find("{") != std::string::npos)
				 		brackets(temp.substr(temp.find(value), std::string::npos), "location", server);
					else
						std::cout << "No opening bracket on the same line as location!" << std::endl;
					break;
				default:
					break;
			}
		}
	}
	return (server);
}

int	readconfig(std::string name, ServerManager &manager)
{
	std::ifstream	configfile(name);
	std::string		temp;
	int				i = 0;

	for (std::string line; std::getline(configfile, line);)
	{
		if (i > 0)
			temp += line + "\n";
		if (line.find("server") != std::string::npos && line.find("{") != std::string::npos)
			i++;
	}
	for (int info = 1; info <= i; info++)
	{
		ServerInfo server;
		temp = temp.substr(brackets(temp, "server", server), std::string::npos);
		manager.setnew_info(server);
		//std::cout << server.getlocation() << std::endl;
	}
	//std::cout << manager.get_info()[0].getlocation() << std::endl;
	//std::cout << manager.get_info()[0].get_ip() << std::endl;
	return (0);
}
