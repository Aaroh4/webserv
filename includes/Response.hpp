#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerInfo.hpp"
#include <filesystem>
#include <cerrno>
#include <random>
#include <sstream>
#include <iomanip>

class Response: public Request
{
	public:
			Response();
			Response(Request &request);
			~Response();
			Response(const Response &);
			Response operator=(const Response &);

			std::string 	formatGetResponseMsg(int code);
			void			sendErrorResponse(std::string errorMessage, int clientfd, int errorCode);
			void			openFile(ServerInfo server);
			void			sendCustomErrorPage(int clientfd);
      		std::string		buildDirectorylist(std::string name, int rootsize);
			void			directorylisting(int clientfd, std::string file);
			void			respond(int clientfd, ServerInfo server);
			void			respondGet(int clientfd, ServerInfo server);
			void			respondPost(int clientfd, ServerInfo server);
			void			respondDelete(int clientfd);
			void			handleCRUD(int clientfd, ServerInfo server);
			void 			sendStandardErrorPage(int sanitizeStatus, int clientfd);
			void			setResponseBody(std::string body);

			// RESPONSE EXCEPTIONS
			//base class for 500 internal error
			class ResponseException: public std::exception
			{
				public:
				    	virtual const char *what() const noexcept;
						virtual int responseCode() const;
			};

			//Bad request
			class ResponseException400: public ResponseException
			{
				public:
					const char *what() const noexcept override;
					int responseCode () const override;
			};

			//Forbidden
			class ResponseException403: public ResponseException
			{
				public:
					const char *what() const noexcept override;
					int responseCode () const override;
			};

			//Not found
			class ResponseException404: public ResponseException
			{
				public:
					const char *what() const noexcept override;
					int responseCode () const override;
			};

			//Method not allowed
			class ResponseException405: public ResponseException
			{
				public:
					const char *what() const noexcept override;
					int responseCode () const override;
			};

			//Unsupported media type
			class ResponseException515: public ResponseException
			{
				public:
					const char *what() const noexcept override;
					int responseCode () const override;
			};

			//Unsupported method
			class ResponseException501: public ResponseException
			{
				public:
					const char *what() const noexcept override;
					int responseCode () const override;
			};

			//Unsupported HTTP
			class ResponseException505: public ResponseException
			{
				public:
					const char *what() const noexcept override;
					int responseCode () const override;
			};


	private:
			ServerInfo	_server;
			std::string _fileSize;
			std::string _contentType;
			std::string _response;
			std::string _responseBody;
			std::string _errorMessage;
			std::fstream _file;
			std::streampos _fsize;
};


#endif
