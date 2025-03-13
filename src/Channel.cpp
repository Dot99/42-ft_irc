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

void Channel::addOperator(Client *client)
{
	_operators.push_back(client);
}

void Channel::addInvitedUser(Client *client)
{
	_invitedUsers.push_back(client);
}

void Channel::removeOperator(Client *client)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
		{
			_operators.erase(_operators.begin() + i);
			break;
		}
	}
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

void Channel::setInviteOnly(bool inviteOnly)
{
	_inviteOnly = inviteOnly;
}

void Channel::setMode(std::string mode)
{
	_mode = mode;
}

void Channel::setPassword(std::string password)
{
	_password = password;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void Channel::setLimit(int limit)
{
	_limit = limit;
}

Client *Channel::getInvitedUser(int fd)
{
	for (size_t i = 0; i < _invitedUsers.size(); i++)
	{
		if (_invitedUsers[i]->getFd() == fd)
		{
			return _invitedUsers[i];
		}
	}
	return NULL;
}

int Channel::getLimit() const
{
	return _limit;
}

std::string Channel::getMode() const
{
	return _mode;
}

std::string Channel::getPassword() const
{
	return _password;
}

std::string Channel::getTopic() const
{
	return _topic;
}

bool Channel::getInviteOnly() const
{
	return _inviteOnly;
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
