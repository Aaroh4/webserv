#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP


class ServerInfo
{
	public:
			ServerInfo();
			ServerInfo(ServeInfo &);
			ServerInfo operator=(ServerInfo &);
			~ServerInfo();
	private:
			unsigned int _ip;
};

#endif