#include "../includes/configExceptions.hpp"

const char* wrongPort::what() const noexcept
{
	 return "Wrong port!";
}

const char* wrongBodyLimit::what() const noexcept
{
    return "Wrong BodyLimit!";
}

const char* wrongTimeout::what() const noexcept
{
    return "Wrong Timeout!";
}

const char* wrongIP::what() const noexcept
{
	return "Wrong IP!";
}

const char* noPermissions::what() const noexcept
{
	return "No Permissions to open.";
}

const char* noServers::what() const noexcept
{
	return "No servers!";
}

const char* serverNotFilled::what() const noexcept
{
	return "Necessary confs not set!";
}
