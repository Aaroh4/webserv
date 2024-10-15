#ifndef CONFIGEXCEPTIONS_HPP
# define CONFIGEXCEPTIONS_HPP

#include <unistd.h>

class wrongPort : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "Wrong port!";
    }
};

class wrongBodyLimit : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "Wrong BodyLimit!";
    }
};

class wrongIP : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "Wrong IP!";
    }
};

class noPermissions : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "No Permissions to open.";
    }

};

#endif
