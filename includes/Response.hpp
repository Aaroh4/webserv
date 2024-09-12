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

			void	respond(int clientf);
	private:

};


#endif