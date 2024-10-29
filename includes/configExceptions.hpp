#ifndef CONFIGEXCEPTIONS_HPP
# define CONFIGEXCEPTIONS_HPP

#include <unistd.h>

class wrongPort : public std::exception
{
    public:
    const char* what() const noexcept override;
};

class wrongBodyLimit : public std::exception
{
    public:
    const char* what() const noexcept override;
};

class wrongTimeout : public std::exception
{
    public:
    const char* what() const noexcept override;
};

class wrongIP : public std::exception
{
    public:
    const char* what() const noexcept override;
};

class noPermissions : public std::exception
{
    public:
    const char* what() const noexcept override;

};

class noServers : public std::exception
{
    public:
    const char* what() const noexcept override;

};

class serverNotFilled : public std::exception
{
    public:
    const char* what() const noexcept override;

};

#endif
