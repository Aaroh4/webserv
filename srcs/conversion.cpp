#include "../includes/ServerInfo.hpp"

unsigned int convertip(std::string ip)
{
	unsigned int returnip[4];
	
	int i = 0;
	while (i < 4)
	{	
		returnip[i] = stoi(ip.substr(0, ip.find('.')));
		ip.erase(0, ip.find('.') + 1);
		i++;
	}
	return ((returnip[0] << 24) | (returnip[1] << 16) | (returnip[2] << 16) | (returnip[3]));
}
