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

			std::fstream	directorylist(std::string name);
			void			respond(int clientfd, ServerInfo server);
			void			respondGet(int clientfd, ServerInfo server);
			void			respondPost(int clientfd);
			void			respondDelete(int clientfd);
			std::string 	getStatusMessage(int statusCode);
			std::string 	formatGetResponseMsg( void );
			void			sendErrorResponse( int clientfd );
			void			openFile(std::string filePath);
			void 			sendNotFound(int clientfd);
			void			sendCustomError(int clientfd);

			// Response exception
			class ResponseException: public std::exception
			{
				public:
					ResponseException(const std::string& message);
					virtual ~ResponseException() noexcept;;
					virtual const char* what() const noexcept override;
				private:
					std::string _message;
			};

	private:

			std::string _fileSize;
			std::string _contentType;
			std::string _response;
			std::string _body;
			std::string _errorMessage;
			std::fstream _file;
			std::streampos _fsize;
};


#endif
