#include "Channel.hpp"

Channel::Channel(std::string name)
{
    _name = name;
}

Channel::~Channel()
{
}

void Channel::addUser(Client *client)
{
    _users.push_back(client);
}	

void Channel::removeUser(Client *client)
{
    for (size_t i = 0; i < _users.size(); i++)
    {
        if (_users[i] == client)
        {
            _users.erase(_users.begin() + i);
            break;
        }
    }
}

std::string Channel::getName() const
{
    return _name;
}

std::vector<Client *> Channel::getUsers() const
{
    return _users;
}

/**
 * @brief Get the User Fd object
 * 
 * @param fd File descriptor
 * @return std::vector<Client *> Users
*/
Client *Channel::getUserFd(int fd)
{
    for (size_t i = 0; i < _users.size(); i++)
    {
        if (_users[i]->getFd() == fd)
        {
            return _users[i];
        }
    }
    return NULL;
}
