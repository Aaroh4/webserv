#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerInfo.hpp"
#include <filesystem>

class Response: public Request
{
	public:
			Response();
			Response(Request &request);
			~Response();
			Response(const Response &);
			Response operator=(const Response &);

			std::string directorylist(std::string name, int rootsize);
			void		directorylisting(int clientfd, ServerInfo server, std::string file);
			void		handleCgi(std::string path, int client_socket);
			void		respond(int clientfd, ServerInfo server);
			void		respondGet(int clientfd, ServerInfo server);
			void		respondPost(int clientfd, ServerInfo server);
			void		respondDelete(int clientfd);
	private:

};

std::string getStatusMessage(int statusCode);

#endif
