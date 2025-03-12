#include "Channel.hpp"

Channel::Channel(std::string name)
{
    _name = name;
}

Channel::~Channel()
{
}

void Channel::addClient(Client *client)
{
    _clients.push_back(client);
}
 
void Channel::removeClient(Client *client)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i] == client)
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }
}

std::string Channel::getName() const
{
    return _name;
}

std::vector<Client *> Channel::getClients() const
{
    return _clients;
}

