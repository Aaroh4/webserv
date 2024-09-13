#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"

class Response: public Request
{
	public:
			Response();
			Response(Request &request);
			~Response();
			Response(const Response &);
			Response operator=(const Response &);

			void	respond(int clientfd);
			void	respondGet(int clientfd);
			void	respondPost(int clientfd);
			void	respondDelete(int clientfd);
	private:

};


#endif