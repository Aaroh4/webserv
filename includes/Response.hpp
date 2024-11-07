#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerInfo.hpp"
#include <filesystem>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <sys/socket.h>

class Response: public Request
{
	public:
			Response();
			Response(Request &request);
			~Response();
			Response(const Response &);
			Response operator=(const Response &);

			std::string 	formatGetResponseMsg(int code);
			void			sendErrorResponse(std::string errorMessage, int clientfd, int errorCode, ServerInfo);
			void			sendCustomErrorPage(int clientfd);
      		std::string		buildDirectorylist(std::string name, int rootsize);
			void			directorylisting(int clientfd, std::string file);
			void			respond(int clientfd, ServerInfo server);
			void			respondGet(int clientfd, ServerInfo server);
			void			respondPost(int clientfd);
			void			respondDelete(int clientfd);
			void			handleCRUD(int clientfd, ServerInfo server);
			void 			sendStandardErrorPage(int sanitizeStatus, int clientfd, ServerInfo server);
			void			setResponseBody(std::string body);
			std::string		formatPostResponseMsg(int close);
			std::string 	formatSessionCookie( void );
			static void     sendErrorPage(int statusCode, int clientfd, std::string body);
			void 			cgiResponse(int clientfd);

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
			class ResponseException415: public ResponseException
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
			std::string _redirectplace;
			std::string _contentType;
			std::string _response;
			std::string _responseBody;
			std::string _errorMessage;
			std::string _fileSize;
};


#endif
