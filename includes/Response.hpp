#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include <filesystem>

class Response: public Request
{
	public:
			Response();
			Response(Request &request);
			~Response();
			Response(const Response &);
			Response operator=(const Response &);

			void			respond(int clientf);
			std::fstream	directorylist();
	private:

};


#endif