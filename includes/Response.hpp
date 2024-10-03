#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerInfo.hpp"
#include <filesystem>
#include <cerrno>

class Response: public Request
{
	public:
			Response();
			Response(Request &request);
			~Response();
			Response(const Response &);
			Response operator=(const Response &);

			std::string 	getStatusMessage(int statusCode);
			std::string 	formatGetResponseMsg(int code);
			void			sendErrorResponse( int clientfd );
			void			openFile(std::string filePath, ServerInfo server);
			void 			sendNotFound(int clientfd);
			void			sendCustomError(int clientfd);
      std::string			buildDirectorylist(std::string name, int rootsize);
			void			directorylisting(int clientfd, std::string file);
			void			handleCgi(std::string path, int client_socket);
			void			respond(int clientfd, ServerInfo server);
			void			respondGet(int clientfd, ServerInfo server);
			void			respondPost(int clientfd, ServerInfo server);
			void			respondDelete(int clientfd);

			// Response exception
			class ResponseException: public std::exception
			{
				public:
				    	const char *what() const throw();
			};


	private:
			ServerInfo	_server;
			std::string _fileSize;
			std::string _contentType;
			std::string _response;
			std::string _body;
			std::string _errorMessage;
			std::fstream _file;
			std::streampos _fsize;
};


#endif
