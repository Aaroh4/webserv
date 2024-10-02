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
			std::string 	formatGetResponseMsg( void );
			void			sendErrorResponse( int clientfd );
			void			openFile(std::string filePath, ServerInfo server);
			void 			sendNotFound(int clientfd);
			void			sendCustomError(int clientfd);
      std::string directorylist(std::string name, int rootsize);
			void		directorylisting(int clientfd, ServerInfo server, std::string file);
			void		handleCgi(std::string path, int client_socket);
			void		respond(int clientfd, ServerInfo server);
			void		respondGet(int clientfd, ServerInfo server);
			void		respondPost(int clientfd, ServerInfo server);
			void		respondDelete(int clientfd);

			// Response exception
			class ResponseException: public std::exception
			{
				public:
				    	const char *what() const throw();
			};

			class ResponseError404: public ResponseException
			{
				public:
				    	const char *what() const throw();
			};

			class ResponseError500: public ResponseException
			{
				public:
				    	const char *what() const throw();
			};

			class ResponseError501: public ResponseException
			{
				public:
				    	const char *what() const throw();
			};

			class ResponseError400: public ResponseException
			{
				public:
				    	const char *what() const throw();
			};

			class ResponseError403: public ResponseException
			{
				public:
				    	const char *what() const throw();
			};

	private:

			std::string _fileSize;
			std::string _contentType;
			std::string _response;
			std::string _responseBody;
			std::string _errorMessage;
			std::fstream _file;
			std::streampos _fsize;
};


#endif
