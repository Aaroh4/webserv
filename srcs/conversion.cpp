#include "../includes/ServerInfo.hpp"
#include "../includes/configExceptions.hpp"

unsigned int convertip(std::string ip)
{
	unsigned int returnip[4];
	
	int i = 0;
	while (i < 4)
	{	
		try
		{
			returnip[i] = stoi(ip.substr(0, ip.find('.')));
		}
	catch (std::exception &e)
	{
		throw(wrongIP());
	}
		ip.erase(0, ip.find('.') + 1);
		i++;
	}
	return ((returnip[0] << 24) | (returnip[1] << 16) | (returnip[2] << 16) | (returnip[3]));
}
