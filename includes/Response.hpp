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
			void	respond(int clientfd, ServerInfo server);
			void	respondGet(int clientfd, ServerInfo server);
			void	respondPost(int clientfd);
			void	respondDelete(int clientfd);
			std::string getStatusMessage(int statusCode);
			std::string formatGetResponseMsg( void );

			// Response exceptions
			class ResponseException: public std::exception
			{
				public:
					ResponseException(std::string const &message) throw();
					virtual ~ResponseException() throw();
					virtual const char *what() const throw();
				private:
					std::string _message;
			};

			class ResponseException404: public ResponseException
			{
				public:
					ResponseException404(std::string const &message) throw();
					virtual ~ResponseException404() throw();
					virtual const char *what() const throw();
				private:
					std::string _message;
			};

			class ResponseException500: public ResponseException
			{
				public:
					ResponseException500(std::string const &message) throw();
					virtual ~ResponseException500() throw();
					virtual const char *what() const throw();
				private:
					std::string _message;
			};

			class ResponseException400: public ResponseException
			{
				public:
					ResponseException400(std::string const &message) throw();
					virtual ~ResponseException400() throw();
					virtual const char *what() const throw();
				private:
					std::string _message;
			};

			class ResponseException403: public ResponseException
			{
				public:
					ResponseException403(std::string const &message) throw();
					virtual ~ResponseException403() throw();
					virtual const char *what() const throw();
				private:
					std::string _message;
			};

			class ResponseException405: public ResponseException
			{
				public:
					ResponseException405(std::string const &message) throw();
					virtual ~ResponseException405() throw();
					virtual const char *what() const throw();
				private:
					std::string _message;
			};

	private:

};


#endif
